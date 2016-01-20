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
		fprintf(stdout, "%% ");

		char *token, *string, *tofree;

		char input[INPUT_LIMIT];

		fgets(input, INPUT_LIMIT, stdin);

		tofree = string = strdup(input);

		if (string == NULL)
			exit(-1);

		while ((token = strsep(&string, " ")) != NULL) {
			pid_t pid = fork();

			if(pid == 0) {
				execlp("ls", "child-name", NULL);
			} else {
				wait(NULL);
			}
		}

		free(tofree);
	}

	printf("Bye!\n");
	exit(0);
}
