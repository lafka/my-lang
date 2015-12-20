#include <sexp.h>
#include <sexp_ops.h>

typedef struct dict {
	char varname[255];
	sexp_t *valexp;
	struct dict *next;
} dict_t;

int ml_eval(char **, char[BUFSIZ], dict_t *);
void ml_purge_env(dict_t *);
