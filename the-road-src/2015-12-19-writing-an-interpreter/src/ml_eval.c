#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "ml_eval.h"

dict_t *insert(char *ref, sexp_t *val, dict_t *env) {
	dict_t *_env = env;

	while (1) {
		if (strcmp(_env->varname, ref) == 0) {
			destroy_sexp(_env->valexp);
			_env->valexp = val;
			return env;
		}

		if (_env->next == NULL)
			break;

		_env = _env->next;
	}

	_env->next = (dict_t *) malloc(sizeof(dict_t));
	_env->next->valexp = val;
	strcpy(_env->next->varname, ref);
	_env->next->next = NULL;

	return env;
}

bool lookup(char *ref, dict_t *env, sexp_t **ret) {
	dict_t *_env = env;

	while (_env != NULL) {
		if (strcmp(ref, _env->varname) == 0) {
			*ret = _env->valexp;
			break;
		}

		_env = _env->next;
	}
}

void ml_dump_env(dict_t *env, char resp[BUFSIZ]) {
	dict_t *_env = env;
	dict_t *next;

	while (_env != NULL) {
		next  = _env->next;

		char dbgbuf[BUFSIZ];

		print_sexp(dbgbuf, BUFSIZ, _env->valexp);
		printf("env/out: %s -> %s\n", _env->varname, dbgbuf);
		_env = next;
	}
}

void ml_purge_env(dict_t *env) {
	dict_t *_env = env;
	dict_t *next;

	while (_env != NULL) {
		next  = _env->next;
		destroy_sexp(_env->valexp);
		free(_env);
		_env = next;
	}
}

int ml_eval(char **input, char resp[BUFSIZ], dict_t *env) {
	size_t len = strlen(*input);
	sexp_t *sx = parse_sexp(*input, len);
	char *v;

	memset(resp, 0, sizeof(*resp));

	if (sx->ty != SEXP_LIST || sx->list->ty != SEXP_VALUE) {
		print_sexp(resp, BUFSIZ, sx);
		return 0;
	}

	v = sx->list->val;

	if (strcmp(v, "define") == 0) {
		insert(sx->list->next->val, sx->list->next->next, env);
		print_sexp(resp, BUFSIZ, sx->list->next->next);
		sx->list->next->next = NULL;
	} else if (strcmp(v, "env") == 0) {
		sexp_t *out, *vlist, *vptr;
		out = new_sexp_list(NULL);

		dict_t *_env = env;
		dict_t *next;

		char r2[BUFSIZ];
		memset(r2, 0, BUFSIZ);
		while (_env != NULL) {
			next  = _env->next;

			vlist = new_sexp_list(NULL);

			if (out->list == NULL) {
				out->list = vptr = vlist;
			} else {
				vptr->next = vlist;
				vptr = vlist;
			}

			vlist->list = new_sexp_atom(_env->varname, strlen(_env->varname), SEXP_BASIC);
			vlist->list->next = _env->valexp;

			_env = next;
		}


		print_sexp(resp, BUFSIZ, out);
	} else {
		sexp_t *ret = NULL;

		lookup(v, env, &ret);
		print_sexp(resp, BUFSIZ, ret);
	}

	return 0;
}
