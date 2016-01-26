/* ch.c --- Un shell pour les hélvètes.  */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define INPUT_LIMIT 128
#define please_dont_segfault(segfaulty_stuff)							    \
    if(segfaulty_stuff == NULL) {										    \
        printf("FIN DE LA MÉMOIRE. FUYEZ PAUVRES FOUS !! (%s, ligne %d)\n", \
               __func__, __LINE__);										    \
        exit(-1);														    \
    }

char count_args(char *str)
{
	if(str == NULL) {
		return 0;
	}

	int i = 0;
	int args = 1;
	char last_char = ' ';

	while (str[i] != '\0' && str[i] != '|' && str[i] != '<' && str[i] != '>') {
		args += str[i] == ' ' && last_char != ' ';
		last_char = str[i];
		i++;
	}

	args -= last_char == ' ';

	return args;
}

int main(void)
{
	int i;
	while (1) {
		printf("$%s: %% ", get_current_dir_name());

		// Deep magic starts here
		int in = 0, out = 1;

		char *cmd, *string, *tofree;

		char input[INPUT_LIMIT];

		// Check the first char of input for EOF
		char eof = getc(stdin);

		if (eof == EOF)
			break;

		ungetc(eof, stdin);

		// input
		fgets(input, INPUT_LIMIT, stdin);

		// cleans trailing \n
		input[strlen(input) - 1] = '\0';

		tofree = string = strdup(input);

		please_dont_segfault(string);

		int children = 0;

		do {
			cmd = strsep(&string, " ");

			char argc = count_args(string);
			char** argv = malloc(sizeof(char*) * (argc + 2));
			
			please_dont_segfault(argv);
			
			char* token = NULL;
			int i = 0;
			
			argv[0] = cmd;
			for(i = 1; i <= argc; i++) {
				do {
					token = strsep(&string, " ");
				} while(token[0] == '\0');
				
				argv[i] = token;
			}
			argv[argc + 1] = (char*) NULL;

			// Empty statements
			if (strlen(cmd) == 0) {
			}

			// Special case for cd
			if (strcmp(cmd, "cd") == 0) {
				if(argv[1] == NULL) {
					chdir(getenv("HOME"));
				} else {
					chdir(argv[1]);
				}
				continue;
			}
			
			int pipefd[2];
			
			pipe(pipefd);

			out = pipefd[1];
				
			if(fork() == 0) {
				
				// Deep magic continues here
				if(in != 0) {
					dup2(in, 0);
					close(in);
				}
				
				if(string != NULL && out != 1) {
					dup2(out, 1);
					close(out);
				}
				
				execvp(cmd, argv);
				printf("%s: no such file or directory ", cmd);
				printf("exception procedure arguments (EWONTFIX)\n");
				exit(-1);
			}
			
			close(out);
			
			in = pipefd[0];

			if(string != NULL) {
				strsep(&string, " ");
			}
			
			free(argv);
			children++;
			
		} while(string != NULL);
		
		for(i = 0; i < children; i++) {
			wait(NULL);
		}
		
		free(tofree);
	}
	
	printf("Bye!\n");
	exit(0);
}
