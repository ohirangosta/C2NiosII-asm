#include "tree_C.h"
#include "token_C.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define LENGTH_MAX 256
#define VAR_MAX 256
#define NAME_LENGTH_MAX 256

void check_T(struct T_t *);
void check_S(struct S_t *);
void check_D(struct D_t *);
void check_L(struct L_t *);
void check_B(struct B_t *);
void check_C(struct C_t *);
void check_E(struct E_t *);

void internal_error();

extern struct IdTable_t *idtable;

int init_flag=1;

struct VarInfo_t {
	char name[NAME_LENGTH_MAX];
	int init_value;
	int used;  /*0:使用されていない 1:使用された*/
};

struct IdTable_t {
	struct VarInfo_t var[VAR_MAX];
	int n_vars;
};

void init_idtable() {
	int i;
	idtable = malloc(sizeof(struct IdTable_t));
	idtable->n_vars = 0;
}

int lookup_id(char name[]) {
	int i;
	for (i = idtable->n_vars-1; i >= 0; i--) {
		if (strcmp(idtable->var[i].name, name) == 0) {
			idtable->var[i].used=1;
			break;
		}
	}
	return i;
}

int register_id(char name[]) {
	int n = idtable->n_vars;
	if (n >= VAR_MAX) return -2; /* too many ids */
	strcpy(idtable->var[n].name, name);
	idtable->n_vars++;
	return n;
}

void semantic_error() {
	fprintf(stderr, "Semantic Error.\n");
	exit(-1);
}

void semantic_warning(char *name) {
	fprintf(stderr, "Semantic Warning.\n");
	printf("ABOUT %s\n", name);
}

int count = 0;

void check_I(struct I_t *i) {
	int idx;
	if (i->kind == I_ID) {
		idx = lookup_id(i->str);
		if (idx < 0) semantic_error();
		else i->index = idx;
	} else if (i->kind == I_NUM) {
		if (init_flag = 1) {
			idtable->var[count++].init_value = atoi(i->str);
		}
	} else {
		internal_error();
	}
}

void check_O(struct O_t *o) {
	if (o->kind == O_ADD) {
		check_E(o->e);
		check_O(o->o);
	} else if (o->kind == O_SUB) {
		check_E(o->e);
		check_O(o->o);
	} else if (o->kind == O_EPS) {
	} else {
		internal_error();
	}
}

void check_E(struct E_t *e) {
	if (e->kind == E_O) {
	        check_O(e->o);
	} else if (e->kind == E_I) {
		check_I(e->i);
	} else if (e->kind == E_EQ) {
		check_E(e->e_right);
	} else if (e->kind == E_NOT) {
		check_E(e->e_right);
	} else if (e->kind == E_LESS) {
		check_E(e->e_right);
	} else if (e->kind == E_LESSEQ) {
		check_E(e->e_right);
	} else if (e->kind == E_MORE) {
		check_E(e->e_right);
	} else if (e->kind == E_MOREEQ) {
		check_E(e->e_right);
	} else if (e->kind == E_EPS) {
	} else {
		internal_error();
	}
}

void check_G(struct G_t *g) {
	if (g->kind == G_EPS) {
	} else if (g->kind == G_ELSE) {
		check_L(g->l);
	} else {
		internal_error();
	}
}

void check_C(struct C_t *c) {
	int idx;
	if (c->kind == C_IF) {
		check_I(c->i);
		check_E(c->e);
		check_B(c->b);
		check_G(c->g);
	} else if (c->kind == C_ID) {
		idx = lookup_id(c->str);
		if (idx < 0) semantic_error();
		else c->index = idx;
		check_I(c->i);
		check_E(c->e);
	} else if (c->kind == C_WHILE) {
		check_I(c->i);
		check_E(c->e);
		check_B(c->b);
	} else {
		internal_error();
	}
}

void check_B(struct B_t *b) {
	if (b->kind == B_STKAKKO) {
		check_L(b->l);
	} else if (b->kind == B_C) {
		check_C(b->c);
	} else {
		internal_error();
	}
}

void check_L(struct L_t *l) {
	if (l->kind == L_EPS) {
/* do nothing */
	} else if (l->kind == L_CL) {
		check_C(l->c);
		check_L(l->l);
	} else {
		internal_error();
	}
}

void check_F(struct F_t *f) {
	if (f->kind == F_EPS) {
	} else if (f->kind == F_CONMA) {
		if (lookup_id(f->str) >= 0) {
			semantic_error();
		}
		f->index = register_id(f->str);
		check_I(f->i);
		check_E(f->e);
		check_F(f->f);
		init_flag = 0;
	} else {
		internal_error();
	}
}

void check_D(struct D_t *d) {
	if (d->kind == D_INT) {
		if (lookup_id(d->str) >= 0) {
			semantic_error();
		}
		d->index = register_id(d->str);
		check_I(d->i);
		check_E(d->e);
		check_F(d->f);
	} else {
		internal_error();
	}
}

void check_S(struct S_t *s) {
	if (s->kind == S_EPS) {
	} else if (s->kind == S_DS) {
		check_D(s->d);
		check_S(s->s);
	} else {
		internal_error();
	}
}


void check_T(struct T_t *t) {
	check_S(t->s);
	check_L(t->l);
}

void check(struct T_t *tree){
	check_T(tree);
	/*
	int i, j;
	for (i = idtable->n_vars-1; i >= 0; i--) {
		if(idtable->var[i].used==0) semantic_warning(idtable->var[i].name);
	}
	*/
}
