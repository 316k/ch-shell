/* ch.c --- Un shell pour les hélvètes.  */

#define _GNU_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#define INPUT_LIMIT 128
#define please_dont_segfault(segfaulty_stuff)							\
    if(segfaulty_stuff == NULL) {										\
        printf("NullPointerExceptionArgumentProcedure (EASKMYBOSS)\n",	\
               __func__, __LINE__);										\
        exit(-1);														\
    }

/**
 * Count files and subdirectories in directory
 */
int count_dong() {
	DIR *dir = opendir(".");
	struct dirent *ent = NULL;
	int count = 0;
	please_dont_segfault(dir);

	while((ent = readdir(dir)) != NULL) {
		if(ent->d_name[0] != '.')
			count++;
	}
	closedir(dir);

	return count;
}

/**
 * Counts the number of arguments in a command
 */
int count_args(char *str)
{
	if(str == NULL) {
		return 0;
	}

	int i = 0;
	int args = 1;
	char last_char = ' ';

	// Break at end of string or i/o redirect
	while (str[i] != '\0' && str[i] != '|' && str[i] != '<' && str[i] != '>') {

		if(str[i] == ' ' && last_char != ' ')
			args++;

		// Expand dong
		if(str[i] == '*' && last_char == ' '
		   && (str[i + 1] == '\0' || str[i + 1] == ' ')) {
			args += count_dong() - 1;
		}

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

		// Fancy prompt : 'user@host:/current/directory%'
		char* dirname = get_current_dir_name();
		printf("%s@%s:%s%% ", getenv("LOGNAME"), getenv("HOSTNAME"), dirname);
		free(dirname);

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
			// Parse command line and create all required subprocesses

			cmd = strsep(&string, " ");

			int argc = count_args(string);

			char** argv = malloc(sizeof(char*) * (argc + 2));

			please_dont_segfault(argv);

			char* token = NULL;
			int i = 0;

			argv[0] = cmd;
			for(i = 1; i <= argc; i++) {

				do {
					// Consume redondant spaces
					token = strsep(&string, " ");
				} while(token[0] == '\0');

				if(strcmp(token, "*") == 0) {
					// expand dong
					DIR *dir = opendir(".");

					please_dont_segfault(dir);

					struct dirent *dong = NULL;

					while((dong = readdir(dir)) != NULL) {
						if(dong->d_name[0] != '.') {
							argv[i] = dong->d_name;
							i++;
						}
					}
					i--;

					closedir(dir);
				} else {
					argv[i] = token;
				}
			}

			argv[argc + 1] = (char*) NULL;

			// Ignore empty statements
			if (strlen(cmd) == 0) {
				goto free_vars;
			}

			// Special case for cd
			if (strcmp(cmd, "cd") == 0) {
				if(argv[1] == NULL) {
					chdir(getenv("HOME"));
				} else {
					chdir(argv[1]);
				}
				free(argv);
				goto free_vars;
			}

			int pipefd[2];

			pipe(pipefd);

			out = pipefd[1];

			char out_to_file = 0;

			while(string != NULL && (string[0] == '<' || string[0] == '>')) {
				char redirect_input = string[0] == '<';

				token = strsep(&string, " ");

				char* filename = strsep(&string, " ");

				if(redirect_input) {
					if(access(filename, F_OK) == -1) {
						// If input file doesn't exist
						printf("%s: no such file or directory ", filename);
						printf("exception procedure arguments (EDONTCARE)\n");
						goto free_vars;
					}

					in = open(filename, O_RDONLY);
				} else {
					out_to_file = 1;
					/* If output file doesn't exist, create it with the
					   right permissions */
					out = open(filename, O_WRONLY | O_CREAT | O_TRUNC,
							   S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
				}
			}

			if(fork() == 0) {

				// Deep magic continues here
				if(in != 0) {
					dup2(in, 0);
					close(in);
				}

				if((string != NULL && out != 1) || out_to_file) {
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
				// Next item should be a pipe... destroy it !
				strsep(&string, " ");
			}

			free(argv);
			children++;

			// repeat until all processes have been forked
		} while(string != NULL);

		// Main process waits for all children to terminate
		for(i = 0; i < children; i++) {
			wait(NULL);
		}

	free_vars:
		free(tofree);
	}

	printf("Bye!\n");
	exit(0);
}
