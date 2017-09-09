#define LENG 10

struct T_t {
	struct S_t *s;
	struct L_t *l;
};

struct S_t {
	int kind;
	struct D_t *d;
	struct S_t *s;
};

struct D_t {
	int kind;
	char str[LENG];
	struct I_t *i;
	struct E_t *e;
	struct F_t *f;
	int index;
};

struct L_t {
	int kind;
	struct D_t *d;
	struct C_t *c;
	struct L_t *l;
};

struct B_t {
	int kind;
	struct L_t *l;
	struct C_t *c;
};

struct C_t {
	int kind;
	char str[LENG];
	struct E_t *e;
	struct I_t *i;
	struct B_t *b;
	struct G_t *g;
	int index;
};

struct G_t {
	int kind;
	struct L_t *l;
};

struct E_t {
	int kind;
	char str[LENG];
	struct O_t *o;
	struct I_t *i;
	struct E_t *e_right;
	int index;
};

struct O_t {
	int kind;
	char str[LENG];
	struct O_t *o;
	struct E_t *e;
	int index;
};

struct I_t {
	int kind;
	char str[LENG];
	int index;
};

struct F_t {
	int kind;
	char str[LENG];
	int index;
	struct I_t *i;
	struct F_t *f;
	struct E_t *e;
};
