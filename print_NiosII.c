#include "tree_C.h"
#include "token_C.h"
#include <stdio.h>
#include <stdlib.h>

void print_T(struct T_t *);
void print_S(struct S_t *);
void print_D(struct D_t *);
void print_L(struct L_t *);
void print_B(struct B_t *);
void print_C(struct C_t *);
void print_E(struct E_t *, int dest);

extern char *yytext;
extern struct IdTable_t *idtable;

#define LENGTH_MAX 256
#define VAR_MAX 256
#define NAME_LENGTH_MAX 256

struct VarInfo_t {
	char name[NAME_LENGTH_MAX];
	int init_value;
	int used;  /*0:使用されていない 1:使用された*/
};

struct IdTable_t {
	struct VarInfo_t var[VAR_MAX];
	int n_vars;
};

void internal_error();

int ivarnum = 2;

int get_ivar() {
	return ivarnum++;
}

int labelnum = 0;
int label_LOOP = 0;
int label_IF = 0;

int get_label() {
	return labelnum++;
}

int get_label_LOOP() {
	return label_LOOP++;
}

int get_label_IF() {
	return label_IF++;
}

void internal_error() {
	fprintf(stderr, "Internal Error.\n");
	exit(-1);
}

void print_I(struct I_t *i, int dest) {
	if (i->kind == I_ID) {
		printf("      ldw r%d, %s(r0)\n", dest, i->str);
	} else if (i->kind == I_NUM) {
		printf("      movi r%d, %s\n", dest, i->str);
	} else {
		internal_error();
	}
}

void print_O(struct O_t *o, int dest) {
	if (o->kind == O_ADD) {
		int arg1 = get_ivar();
		print_E(o->e, arg1);
		print_O(o->o, dest);
		printf("      add r%d, r%d, r%d\n", dest, dest, arg1);
	} else if (o->kind == O_SUB) {
		int arg1 = get_ivar();
		print_E(o->e, arg1);
		print_O(o->o, dest);
		printf("      sub r%d, r%d, r%d\n", dest, dest, arg1);
	} else if (o->kind == O_EPS) {

	} else {
		internal_error();
	}
}

void print_E(struct E_t *e, int dest) {

	if (e->kind == E_I) {
		print_I(e->i, dest);
	} else if (e->kind == E_O) {
		print_O(e->o, dest);
	} else if (e->kind == E_EQ) {
		int arg1 = get_ivar();
		print_E(e->e_right, arg1);
		printf("      beq r%d, r%d", dest, arg1);
	} else if (e->kind == E_NOT) {
		int arg1 = get_ivar();
		print_E(e->e_right, arg1);
		printf("      bne r%d, r%d", dest, arg1);
	} else if (e->kind == E_LESS) {
		int arg1 = get_ivar();
		print_E(e->e_right, arg1);
		printf("      blt r%d, r%d", dest, arg1);
	} else if (e->kind == E_LESSEQ) {
		int arg1 = get_ivar();
		print_E(e->e_right, arg1);
		printf("      ble r%d, r%d", dest, arg1);
	} else if (e->kind == E_MORE) {
		int arg1 = get_ivar();
		print_E(e->e_right, arg1);
		printf("      bgt r%d, r%d", dest, arg1);
	} else if (e->kind == E_MOREEQ) {
		int arg1 = get_ivar();
		print_E(e->e_right, arg1);
		printf("      bge r%d, r%d", dest, arg1);
	} else if (e->kind == E_EPS) {
	} else {
		internal_error();
	}
}

int end_flag = 0;
int else_flag = 0;

void print_G(struct G_t *g, int label, int l_if) {
	if (g->kind == G_EPS) {

	} else if (g->kind == G_ELSE) {
		else_flag = 1;
		printf("ELSE%d:\n", l_if);
		print_L(g->l);
		printf("      br LABEL%d\n", label);
	} else {
		internal_error();
	}
}

void print_C(struct C_t *c) {
	if (c->kind == C_IF) {
		int dest = get_ivar();
		print_I(c->i, dest);
		int l_IF = get_label_IF();
		int l = get_label();
		print_E(c->e, dest);
		printf(", IF%d\n", l_IF);
		print_G(c->g, l, l_IF);
		if (else_flag == 0) {
			printf("      br LABEL%d\n", l);
		}
		printf("IF%d:\n", l_IF);
		print_B(c->b);
		printf("LABEL%d:\n", l);
	} else if (c->kind == C_ID) {
		int dest = get_ivar();
		print_I(c->i, dest);
		print_E(c->e, dest);
		printf("      stw r%d, %s(r0)\n", dest, c->str);
	} else if (c->kind == C_WHILE) {
		int dest = get_ivar();
		int l_LOOP = get_label_LOOP();
		int l = get_label();
		printf("COMP%d:\n", l_LOOP);
		print_I(c->i, dest);
		print_E(c->e, dest);
		printf(", LOOP%d\n", l_LOOP);
		printf("      br LABEL%d\n", l);
		printf("LOOP%d:\n", l_LOOP);
		print_B(c->b);
		printf("      br COMP%d\n", l_LOOP);
		printf("LABEL%d:\n", l);
	} else {
		internal_error();
	}
        ivarnum = 2;
}	

void print_B(struct B_t *b) {
	if (b->kind == B_STKAKKO) {
		print_L(b->l);
	} else if (b->kind == B_C) {
		print_C(b->c);
	} else {
		internal_error();
	}
}

void print_L(struct L_t *l) {
	if (l->kind == L_EPS) {
/* do nothing */
	} else if (l->kind == L_CL) {
		print_C(l->c);
		print_L(l->l);
	} else {
		internal_error();
	}
}

void print_F(struct F_t *f, int dest) {
	dest = get_ivar();
	if (f->kind == F_CONMA) {
		print_I(f->i, dest);
		print_E(f->e, dest);
		print_F(f->f, dest);
		printf("      stw r%d, %s(r0)\n", dest, f->str);
	} else if (f->kind == F_EPS) {
	} else {
		internal_error();
	}
}

void print_D(struct D_t *d) {
	int dest = get_ivar();
	if (d->kind == D_INT) {
		print_I(d->i, dest);
		print_E(d->e, dest);
		print_F(d->f, dest);
		printf("      stw r%d, %s(r0)\n", dest, d->str);
	} else {
		internal_error();
	}
}

void print_S(struct S_t *s) {
	if (s->kind == S_EPS) {
	} else if (s->kind == S_DS) {
		print_D(s->d);
		print_S(s->s);
	} else {
		internal_error();
	}
}

void print_T(struct T_t *t) {
	print_S(t->s);
	ivarnum = 2;
	print_L(t->l);
}

void prologue() {
	printf(".text\n");
	printf(".global _start\n");
	printf("_start:\n");
}

void epilogue() {
	int i = 0;
	printf("END:\n      br END\n\n");
	printf(".data\n");
	while (i<idtable->n_vars) {
		printf("%s:   .word %d\n", idtable->var[i].name, idtable->var[i].init_value);
		i++;	
	}
}

void codegen(struct T_t *tree) {
	prologue();
	print_T(tree);
	epilogue();
}
