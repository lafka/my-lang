#include <stdio.h>
#include <stdlib.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "ml_eval.h"

#define HISTFILE "/tmp/phistory"

char *ml_prompt_getline() {
	char *buf = readline("> ");

	if (buf == NULL) {
		printf("\n");
	} else if(strlen(buf) == 0) {
	} else {
		add_history(buf);
		append_history(1, HISTFILE);
	}

	return buf;
}

int main(int argc, char** argv) {

	char *input = (char *) NULL;
	char resp[BUFSIZ];
	int evalerr = 0;

	puts("The Prompt - v0.0.0-1");
	puts("Ctrl+c to exit!\n");

	if (2 == read_history(HISTFILE)) {
		FILE *fd = fopen(HISTFILE, "w+");
		if (fd > 0 )
			fclose(fd);
		else
			exit(1);
	}

	dict_t *env = (dict_t *) malloc(sizeof(dict_t));
	memset(env, 0, sizeof(dict_t));

	while (1) {
		input = ml_prompt_getline();
		evalerr = ml_eval(&input, resp, env);

		if (0 != evalerr) {
			printf("ERROR[%d] %s\n", evalerr, *resp);
		} else {
			printf("#> %s\n", resp);
		}

		free(input);
	}

	ml_purge_env(env);

	history_truncate_file(HISTFILE, 1000);

	return 0;
}

