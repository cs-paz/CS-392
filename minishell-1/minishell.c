/**
Christian Szablewski-Paz and Zac Schuh
I pledge my honor that I have abided by the Stevens Honor System.
*/

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <pwd.h>
#include <wait.h>
#include <setjmp.h>

#define BRIGHTBLUE "\x1b[34;1m"
#define DEFAULT "\x1b[0m"

sigjmp_buf jmpbuf;
pid_t check;

void catch_signal(int sig) {
	int stat;
	if (waitpid(check, &stat, WNOHANG)) {
		write(STDOUT_FILENO, "\n", 1);
	}
	siglongjmp(jmpbuf, 1);
}

int display_cwd() {
	char buf[PATH_MAX];
	if (getcwd(buf, PATH_MAX * sizeof(char)) == NULL) {
		fprintf(stderr, "Error: Cannot get current working directory. %s.\n", strerror(errno));
		return EXIT_FAILURE;
	}
	printf("[%s%s%s]$ ", BRIGHTBLUE, buf, DEFAULT);
	memset(buf, 0, sizeof(buf));
	return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
	if (argc != 1) {
		fprintf(stderr, "Usage: %s\n", argv[0]);
		return EXIT_FAILURE;
	}

	struct sigaction sa;
	sa.sa_handler = catch_signal;    
	sa.sa_flags = SA_RESTART; 

	if (sigaction(SIGINT, &sa, NULL) == -1) {        
		fprintf(stderr, "Error: Cannot register signal handler. %s.\n", strerror(errno));        
		return EXIT_FAILURE;    
	}

	sigsetjmp(jmpbuf, 1);
	
	int input_size = 1024;

	char input[input_size];
	while (true) {
		display_cwd();
		if(fgets(input, input_size - 1, stdin) == NULL){
			fprintf(stderr, "Error: Failed to read from stdin. %s.\n", strerror(errno));
		}
		input[strlen(input) - 1] = '\0';

		if (strcmp(input, "") == 0) {
			continue;
		}
		if (strcmp(input, "exit") == 0) {
			break;
		}		
		else if (strcmp(input, "cd") == 0 || strcmp(input, "cd ~") == 0 || strcmp(input, "cd ") == 0) {
			uid_t uid = getuid();
			struct passwd *password;
			if ((password = getpwuid(uid)) == NULL) {
				fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno));
				continue;
			}
			char home_dir[PATH_MAX];
			strcpy(home_dir, password->pw_dir);
			if (chdir(home_dir) == -1) {
				fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", home_dir, strerror(errno));
				continue;
			}
		}
		else if (strncmp(input, "cd ", 3) == 0) {
			uid_t uid = getuid();
			struct passwd *password;
			if ((password = getpwuid(uid)) == NULL) {
				fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno));
				continue;
			}
			char home_dir[PATH_MAX];
			strcpy(home_dir, password->pw_dir);

			char *new_dir = input;
			new_dir += 3;
			if (strchr(new_dir, ' ') != NULL) {
				fprintf(stderr, "Error: Too many arguments to cd.\n");
				continue;
			}
			if (new_dir[0] == '~') {
				char cat = '/';				
				new_dir += 2;
				strncat(home_dir, &cat, 1);
				strcat(home_dir, new_dir);
				chdir(home_dir);
			}
			else if (chdir(new_dir) == -1) {
				fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", new_dir, strerror(errno));
				continue;
			}
		}
		else {
			pid_t pid;
			if ((pid = fork()) < 0) {
				fprintf(stderr, "Error: fork() failed. %s.\n", strerror(errno));
				continue;
			}
			else if (pid == 0) {
				if (sigaction(SIGINT, &sa, NULL) == -1) {        
					fprintf(stderr, "Error: Cannot register signal handler. %s.\n", strerror(errno));        
					return EXIT_FAILURE;    
				}

				char _args[1024][1024];
				int count = 0, temp = 0;
				for (int i = 0; i < strlen(input); i++) {
					if (input[i] == ' ' || input[i] == '\0') {
						_args[count][temp] = '\0';
						temp = 0;
						count++;
					}
					else {
						_args[count][temp] = input[i];
						temp++;
					}
				}

				char **args;
				if ((args = (char **)malloc((count + 1) * sizeof(char *))) == NULL) {
					fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
					return EXIT_FAILURE;
				}
				for (int i = 0; i < count + 1; i++) {
					args[i] = _args[i];
				}		
				char *first = args[0];
				first[strlen(first)] = '\0';		
				args[count + 1] = NULL;

				if ((execvp(first, args)) == -1) {
					fprintf(stderr, "Error: exec() failed. %s.\n", strerror(errno));
					return EXIT_FAILURE;
				}
				
				for (int i = 0; i < count; i++) {
					free(args[i]);
				}
				free(args);
			}
			else {
				check = pid;
				if (sigaction(SIGINT, &sa, NULL) == -1) {        
					fprintf(stderr, "Error: Cannot register signal handler. %s.\n", strerror(errno));        
					continue;   
				}
				int stat;
				do {
					pid_t w = waitpid(pid, &stat, WUNTRACED | WCONTINUED);
					if (w == -1) {
						fprintf(stderr, "Error: wait() failed. %s.\n", strerror(errno));
						continue;
					}
				} while (!WIFEXITED(stat) && !WIFSIGNALED(stat));
			}
		}
		memset(input, 0, sizeof(input));
	}

	return EXIT_SUCCESS;
}

