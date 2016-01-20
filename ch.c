/* ch.c --- Un shell pour les hélvètes.  */

#include <stdio.h>
#include <stdlib.h>
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
			printf("%s\n", token);
		}

		free(tofree);
	}

	printf("Bye!\n");
	exit(0);
}
