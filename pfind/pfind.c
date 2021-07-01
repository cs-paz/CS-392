/*******************************************************************************
 * Name        : pfind.c
 * Author      : Christian Szablewski-Paz & Zac Schuh
 * Date        : 03/16/21
 * Description : Prints out the permissions of a given directory or file
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include "pfindsource.h"

bool indexIsValid(char e, char perm){
	return (e == perm || e == '-');
}

int main(int argc, char **argv){
    bool dFlag = false;
    bool pFlag = false;
    bool atLestOneOptionSelected = false;
    DIR *dir;
    char path[PATH_MAX];
    int c;
    
    // Parse through command line args
    while((c = getopt(argc, argv, ":d:p:h")) != -1){
        atLestOneOptionSelected = true;
        switch(c) {
            case 'h':
                printf("Usage: ./pfind -d <directory> -p <permissions string> [-h]\n");
                return EXIT_SUCCESS;
            case 'd':
                dFlag = true;
                break;
            case 'p':
                pFlag = true;
                break;
            case '?':
                fprintf(stderr, "Error: Unknown option '-%c' recieved.\n", optopt);
                return EXIT_FAILURE;
        }
    }
    
    // Basic Input Validation
    if(!atLestOneOptionSelected) {
        printf("Usage: ./pfind -d <directory> -p <permissions string> [-h]\n");
        return EXIT_FAILURE;
    }
    if(dFlag != false && pFlag == false ) {
        fprintf(stderr, "Error: Required argument -p <permissions string> not found.\n");
        return EXIT_FAILURE;
    }
    if(dFlag == false) {
        fprintf(stderr, "Error: Required argument -d <directory> not found.\n");
        return EXIT_FAILURE;
    }
    if(realpath(argv[2], path) == NULL) {
        fprintf(stderr, "Error: Cannot stat '%s'. %s.\n", argv[2], strerror(errno));
        return EXIT_FAILURE;
    }
    if((dir = opendir(path)) == NULL) {
        fprintf(stderr, "Error: Cannot open directory '%s'. %s.\n", path, strerror(errno));
        closedir(dir);
        return EXIT_FAILURE;
    }
    if(strlen(argv[4]) != 9) {
        fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", argv[4]);
        closedir(dir);
        return EXIT_FAILURE;
    }
    
    // Permissions string validation
    // Valid permission string is as follows 
    // rwxrwxrwx
    // ---------
    char *permissions_string = argv[4];
    for(int i = 0; i < 9; i++) {
        if(i % 3 == 0){
            if(!indexIsValid(permissions_string[i], 'r')) {
                fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", permissions_string);
                closedir(dir);
                return EXIT_FAILURE;
            }
        }
        else if(i % 3 == 1){
            if(!indexIsValid(permissions_string[i], 'w')) {
                fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", permissions_string);
                closedir(dir);
                return EXIT_FAILURE;
            }
        }
        else {
            if(!indexIsValid(permissions_string[i], 'x')) {
                fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", permissions_string);
                closedir(dir);
                return EXIT_FAILURE;
            }
        }
    }
    traverse_dir(dir, path, permissions_string);
    return EXIT_SUCCESS;
}
