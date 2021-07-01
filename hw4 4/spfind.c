/*******************************************************************************
 * Name        : spfind.c
 * Author      : Christian Szablewski-Paz & Zac Schuh
 * Date        : 03/31/21
 * Description : runs pfind and sorts it using piping
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <string.h>

int main(int argc, char *argv[]) {

    if(argc == 1) {
        printf("Usage: %s -d <directory> -p <permissions string> [-h]\n", argv[0]);
        return EXIT_SUCCESS;
    }
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            printf("Usage: %s -d <directory> -p <permissions string> [-h]\n", argv[0]);
            return EXIT_SUCCESS;
        }
        if (i > 4 && strncmp(argv[i], "-", 1) == 0) {
            fprintf(stderr, "Error: Unknown option '%s' received.\n", argv[i]);
            return EXIT_FAILURE;
        }
    }
    
    int pfind_to_sort[2], sort_to_parent[2];
    if (pipe(pfind_to_sort) == -1 || pipe(sort_to_parent) == -1) {
        fprintf(stderr, "Error: Pipe Failed. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }
    
    pid_t pids[2];
    if ((pids[0] = fork()) == 0) {
        // error check pfind
        if(close(pfind_to_sort[0]) == -1) {
            fprintf(stderr, "Error: close failed. %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        if(dup2(pfind_to_sort[1], STDOUT_FILENO) == -1) { 
            fprintf(stderr,"Error: dup2 failed. %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        
        if(close(sort_to_parent[0]) == -1 || close(sort_to_parent[1]) == -1) {
            fprintf(stderr, "Error: close failed. %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        
        //run pfind executable
        if (execlp("./pfind", "./pfind", argv[1], argv[2], argv[3], argv[4], NULL) == -1) {
            fprintf(stderr, "Error: pfind failed.\n");
            return EXIT_FAILURE;
        }
    }
    else if ((pids[1] = fork()) == 0) {
        // error check sort
        if(close(pfind_to_sort[1]) == -1) {
            fprintf(stderr, "Error: close failed. %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        if(dup2(pfind_to_sort[0], STDIN_FILENO) == -1) {
            fprintf(stderr,"Error: dup2 failed. %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        if(close(sort_to_parent[0]) == -1) {
            fprintf(stderr, "Error: close failed. %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        if(dup2(sort_to_parent[1], STDOUT_FILENO) == -1) {
            fprintf(stderr,"Error: dup2 failed. %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        
        //run sort executable
        if (execlp("sort", "sort", NULL) == -1) {
            fprintf(stderr, "Error: sort failed.\n");
            return EXIT_FAILURE;
        }
    }
    else if (pids[0] == -1 || pids[1] == -1) {
        fprintf(stderr, "Error: fork failed.\n");
        return EXIT_FAILURE;
    }
    else {
        // error check parent
        if(close(sort_to_parent[1]) == -1) {
            fprintf(stderr, "Error: close failed. %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        if(dup2(sort_to_parent[0], STDIN_FILENO) == -1) {
            fprintf(stderr,"Error: dup2 failed. %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        
        // close the rest of the files
        if(close(pfind_to_sort[0]) == -1) {
            fprintf(stderr, "Error: close failed. %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        if(close(pfind_to_sort[1]) == -1) {
            fprintf(stderr, "Error: close failed. %s\n", strerror(errno));
            return EXIT_FAILURE;
        }
        
        char buf[PATH_MAX + 1];
        int bufIterator = 0;
        int lines_read = 0;
        int bytes_read = 0;
        
        do {
            char char_read = 0;
            bytes_read = read(STDIN_FILENO, &char_read, 1);
            buf[bufIterator++] = char_read;
            if (bytes_read == -1)  { //if read() fails
                perror("read()");
                return EXIT_FAILURE;
            }
            if (char_read == '\n') {
                if(write(1, buf, bufIterator) == -1) {
                    fprintf(stderr, "Error: write failed. %s", strerror(errno));
                    return EXIT_FAILURE;
                }
                bufIterator = 0;
                lines_read++;
            }
        } while (bytes_read > 0);
        
        int status[2];
        if(wait(&status[0]) < 0 || wait(&status[1]) < 0) {
            fprintf(stderr, "Error: wait failed. %s", strerror(errno));
            return EXIT_FAILURE;
        }
        
        if (WEXITSTATUS(status[0]) == 0 && WEXITSTATUS(status[1]) == 0) {
            printf("Total matches: %d\n", lines_read);
        }
    }
    return EXIT_SUCCESS;
}
