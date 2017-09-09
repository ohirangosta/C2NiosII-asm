%{
#include "token_C.h"
%}
%%
"int"			{ return M_INT; }
";"			{ return M_SEMIC; }
","			{ return M_CONMA; }
"+" 			{ return M_ADD; }
"-"			{ return M_SUB; }
"="			{ return M_EQALL; }
"("			{ return M_SKAKKO; }
")"			{ return M_EKAKKO; }
"{"			{ return M_STKAKKO; }
"}"			{ return M_ETKAKKO; }
"if"			{ return M_IF; }
"else"			{ return M_ELSE; }
"while"			{ return M_WHILE; }
"main"			{ return M_MAIN; }
"void"			{ return M_VOID; }
"return"		{ return M_RETURN; }
"=="			{ return M_EQ; }
"!="			{ return M_NOT; }
"<"			{ return M_LESS; }
"<="			{ return M_LESSEQ; }
">"			{ return M_MORE; }
">="			{ return M_MOREEQ; }
" "+ 			{ return M_SPC; }
"\n" 			{ return M_NL; }
[a-zA-Z][a-zA-Z0-9]* 	{ return M_ID; }
0|[1-9][0-9]* 		{ return M_NUM; }
. 			{ return M_UD; }
