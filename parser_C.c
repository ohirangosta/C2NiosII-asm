#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "token_C.h"
#include "tree_C.h"

struct T_t *parse_T();
struct S_t *parse_S();
struct D_t *parse_D();
struct L_t *parse_L();
struct B_t *parse_B();
struct C_t *parse_C();
struct G_t *parse_G();
struct E_t *parse_E();
struct I_t *parse_I();
struct F_t *parse_F();

int yylval;
int yylex();

#define BUF_MAX 256

char buf[BUF_MAX];
char *yytext;

#define M_EMPTY -4

struct IdTable_t *idtable;
int n_processed;
int n_total;
int saved_marker; /*切り出しと消費を区別*/

void init_buf() {
	n_total = 0;
	n_processed = 0;
	saved_marker = M_EMPTY;
}

int scan() {
	int m = yylex();

	return m;
}

int nexttoken() {
	int m;
	if (saved_marker != M_EMPTY) { /* 前回 scan() した結果が残っている */
		m = saved_marker;
	} else { /* 前回 scan() した結果が残っていない */
		m = scan();
		while (m == M_SPC || m == M_NL) { /* M_SPCは読み飛ばす */
			m = scan();
		}
		saved_marker = m;
	}
	return m;
}

void syntax_error() {
	printf("error: before ‘%s’\n", yytext);
	fprintf(stderr, "Syntax Error.\n");
	exit(-1);
}

void eat_token() {
	saved_marker = M_EMPTY;
}

struct I_t *parse_I() {
	struct I_t *i = malloc(sizeof(struct I_t));
	int m = nexttoken();
	if (m == M_ID) {
		eat_token();
		i->kind = I_ID;
		strcpy(i->str, yytext);
	} else if (m == M_NUM) {
		eat_token();
		i->kind = I_NUM;
	        strcpy(i->str, yytext);
	} else {
		syntax_error();
	}	
	return i;
}

struct O_t *parse_O() {
	struct O_t *o = malloc(sizeof(struct O_t));
	int m = nexttoken();
	if (m == M_ADD) {
		eat_token();
		o->kind = O_ADD;
		o->e = parse_E();
		o->o = parse_O();
	} else if (m == M_SUB) {
		eat_token();
		o->kind = O_SUB;
		o->e = parse_E();
		o->o = parse_O();
	} else if (m == M_SEMIC||m == M_CONMA) {
		o->kind = O_EPS;
	} else {
		syntax_error();
	}
	return o;
}

struct E_t *parse_E() {
	struct E_t *e = malloc(sizeof(struct E_t));
	int m = nexttoken();
	if (m == M_ID||m == M_NUM) {
		e->kind = E_I;
		e->i = parse_I();	
	} else if (m == M_ADD||m == M_SUB) {
		e->kind = E_O;
		e->o = parse_O();
	} else if (m == M_EQ) {
		eat_token();
		e->kind = E_EQ;
		e->e_right = parse_E();
	} else if (m == M_NOT) {
		eat_token();
		e->kind = E_NOT;
		e->e_right = parse_E();
	} else if (m == M_LESS) {
		eat_token();
		e->kind = E_LESS;
		e->e_right = parse_E();
	} else if (m == M_LESSEQ) {
		eat_token();
		e->kind = E_LESSEQ;
		e->e_right = parse_E();
	} else if (m == M_MORE) {
		eat_token();
		e->kind = E_MORE;
		e->e_right = parse_E();
	} else if (m == M_MOREEQ) {
		eat_token();
		e->kind = E_MOREEQ;
		e->e_right = parse_E();
	} else if (m == M_CONMA||m == M_SEMIC||m == M_EKAKKO) {
		e->kind = E_EPS;
	} else {
		syntax_error();
	}
	return e;
}

struct G_t *parse_G() {
	struct G_t *g = malloc(sizeof(struct G_t));
	int m = nexttoken();
	if (m == M_ENDOFFILE||m == M_ID||m == M_IF||m == M_WHILE||m == M_ETKAKKO) {
		g->kind = G_EPS;
	} else if (m == M_ELSE){
		eat_token();
		g->kind = G_ELSE;		
		m = nexttoken();
		if (m == M_STKAKKO) {
			eat_token();
			g->l = parse_L();
			m = nexttoken();
			if (m == M_ETKAKKO) {
				eat_token();
			} else {
				syntax_error();
			}
		} else {
			syntax_error();
		}
	} else {
		syntax_error();
	}
	return g;
} 

struct C_t *parse_C() {
	struct C_t *c = malloc(sizeof(struct C_t));
	int m = nexttoken();
	if (m == M_ID) {
		eat_token();
		c->kind = C_ID;
		strcpy(c->str, yytext);
		m = nexttoken();
		if(m == M_EQALL) {
			eat_token();
			c->i = parse_I();
			c->e = parse_E();
			m = nexttoken();
			if (m == M_SEMIC) {
				eat_token();
			} else {
				syntax_error();
			}
		} else {
		        syntax_error();
		}
	} else if (m == M_IF) {
		eat_token();
		c->kind = C_IF;
		m = nexttoken();
		if (m == M_SKAKKO) {
			eat_token();
			c->i = parse_I();
			c->e = parse_E();
			m = nexttoken();
			if (m == M_EKAKKO) {
				eat_token();
				c->b = parse_B(); 
				c->g = parse_G();
			} else {
				syntax_error();
			}
		} else {	
			syntax_error();
		}
	} else if (m == M_WHILE) {
		eat_token();
		c->kind = C_WHILE;
		m = nexttoken();
		if (m == M_SKAKKO) {
			eat_token();
			c->i = parse_I();
			c->e = parse_E();
			m = nexttoken();
			if (m == M_EKAKKO) {
				eat_token();
				c->b = parse_B();
			} else {
				syntax_error();
			}
		} else {	
			syntax_error();
		}
	} else {
		syntax_error();
	}
	return c;
}

struct B_t *parse_B() {
	struct B_t *b = malloc(sizeof(struct B_t));
	int m = nexttoken();
	if(m == M_STKAKKO) {
		eat_token();
		b->kind = B_STKAKKO;
		b->l = parse_L();
		m=nexttoken();
		if(m == M_ETKAKKO) {
			eat_token();
		} else {
			syntax_error();
		}
	} else if(m == M_ID||m == M_IF||m == M_WHILE) {
		b->kind = B_C;
		b->c = parse_C();
	} else {
		syntax_error();
	}
	return b;
}

struct L_t *parse_L() {
	struct L_t *l = malloc(sizeof(struct L_t));
	int m = nexttoken();
	if (m == M_ENDOFFILE|| m == M_ETKAKKO) {
/* do nothing */
		l->kind = L_EPS;
	} else if (m == M_ID||m == M_IF||m == M_WHILE) {
		l->kind = L_CL;
		l->c = parse_C();		
		l->l = parse_L();
	} else {
		syntax_error();
	}
	return l;
}

struct F_t *parse_F() {
	struct F_t *f = malloc(sizeof(struct F_t));
	int m = nexttoken();
	if (m == M_SEMIC) {
		f->kind = F_EPS;
	} else if (m == M_CONMA) {
		eat_token();
		f->kind = F_CONMA;
		m = nexttoken();
		if (m == M_ID) {
			eat_token();
			strcpy(f->str, yytext);
			m = nexttoken();
			if (m == M_EQALL) {
				eat_token();
				m = nexttoken();
				f->i = parse_I();
			        f->e = parse_E();	       
				f->f = parse_F();
			} else {
				syntax_error();
			}
		} else {
			syntax_error();
		}
	} else {
		syntax_error();
	}
	return f;
}

struct D_t *parse_D() {
	struct D_t *d = malloc(sizeof(struct D_t));
	int m = nexttoken();
	if(m == M_INT) {
		eat_token();
		d->kind = D_INT;		
		m = nexttoken();
		if(m == M_ID) {			
			eat_token();
			strcpy(d->str, yytext);
			m = nexttoken();
			if(m == M_EQALL) {
				eat_token();
				d->i = parse_I();				
				d->e = parse_E();				
				d->f = parse_F();
				m=nexttoken();
				if(m = M_SEMIC) {
					eat_token();
				} else {
					syntax_error();
				}
			} else {
				syntax_error();
			}
		} else {
			syntax_error();
		}
	} else {
		syntax_error();
	}
	return d;
}

struct S_t *parse_S() {
	struct S_t *s = malloc(sizeof(struct S_t));
	int m = nexttoken();
	if (m == M_ENDOFFILE||m == M_ID||m == M_IF||m == M_WHILE) {
/* do nothing */ 
		s->kind = S_EPS;
	} else if (m == M_INT) {
		
		s->kind = S_DS;
	        s->d = parse_D();
		s->s = parse_S();
	} else {
		syntax_error();
	}
	return s;
}	

struct T_t *parse_T() {
	struct T_t *t = malloc(sizeof(struct T_t));
	int m = nexttoken();
	if (m == M_INT) {
		eat_token();
		m = nexttoken();
		if (m == M_MAIN) {
			eat_token();
			m = nexttoken();
			if (m == M_SKAKKO) {
				eat_token();
				m = nexttoken();
				if (m == M_VOID) {
					eat_token();
					m = nexttoken();
					if (m == M_EKAKKO) {
						eat_token();
						m = nexttoken();
						if (m == M_STKAKKO) {
							eat_token();
							m = nexttoken();
							if (m == M_ENDOFFILE) {
							} else if (m == M_INT||m == M_ID||m == M_IF||m == M_WHILE) {
								t->s = parse_S();
								t->l = parse_L();
							} else {
								syntax_error();
							}
						}
					}
				}
			}
		}
	}
	return t;
}


struct T_t *parse() {
	struct T_t *tree;
	tree = parse_T(); /* 出発記号に関する解析関数を呼び出す */
	if (nexttoken() != M_ENDOFFILE) syntax_error();
	return tree;
}

int main() {
	init_buf();
	init_idtable();
	struct T_t *tree = parse(); /* 構文解析 */
	check(tree);
	codegen(tree);
	return 0;
}
