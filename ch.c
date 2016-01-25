/* ch.c --- Un shell pour les hélvètes.  */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define INPUT_LIMIT 128

int main(void)
{
	while(1) {
		fprintf(stdout, "$%s: %% ", get_current_dir_name());

		char *cmd, *token, *string, *tofree;

		char input[INPUT_LIMIT];

		// Check the first char of input for EOF
		char eof = getc(stdin);

		if(eof == EOF)
			break;

		ungetc(eof, stdin);

		// input
		fgets(input, INPUT_LIMIT, stdin);

		// cleans trailing \n
		input[strlen(input) - 1] = '\0';

		tofree = string = strdup(input);

		if (string == NULL)
			exit(-1);

		cmd = strsep(&string, " ");

		if(strcmp(cmd, "cd") == 0) {
			chdir(string);
		} else {
			pid_t pid = fork();

			if(pid == 0) {
				execlp(cmd, cmd, string, (char*) NULL);
				printf("%s: no such file or directory exception procedure arguments\n", cmd);
				exit(-1);
			} else {
				wait(NULL);
			}
		}

		free(tofree);
	}

	printf("Bye!\n");
	exit(0);
}
