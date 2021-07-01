/*******************************************************************************
 * Name        : quicksort.h
 * Author      : Christian Szablewski-Paz and Zac Schuh
 * Date        : 3/3/21
 * Description : pfind.c source code
 * Pledge      : I pledge my honor that I have abided by the Stevens honor system.
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

int traverse_dir(DIR *dir, char* path, char pstring[]){

    if(dir == NULL){
        fprintf(stderr, "Error: Cannot open directory '%s'. %s.\n", path, strerror(errno));
        return EXIT_FAILURE;
    }
    
    struct dirent *entry;
    struct stat sb;
    
    char file_name[PATH_MAX];
    size_t pathlen = 0;
    file_name[0] = '\0';
    char perms[10];
    if(strcmp(path, "/")){
        strncpy(file_name, path, PATH_MAX);
    }
    
    pathlen = strlen(file_name) + 1;
    
    file_name[pathlen] = '\0';
    file_name[pathlen-1] = '/';
    
    entry = readdir(dir);
    
    int permissions[] = {S_IRUSR, S_IWUSR, S_IXUSR,
               S_IRGRP, S_IWGRP, S_IXGRP,
               S_IROTH, S_IWOTH, S_IXOTH};
    
    while(entry != NULL){
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
        }
        
        strncpy(file_name + pathlen, entry->d_name, PATH_MAX - pathlen);
        
        if(lstat(file_name, &sb) < 0){
            fprintf(stderr, "Error: Cannot stat file '%s'. %s.\n", file_name, strerror(errno));
            continue;
        }
        
        int i;
		char *vals = "rwxrwxrwx";
	
		for(i = 0; i < 9; i++){
			if(sb.st_mode & permissions[i]){
				perms[i] = vals[i];
			}
			else if(sb.st_mode){
				perms[i] = '-';
			}
		}
		perms[9] = '\0';
        
        if(strcmp(perms, pstring) == 0){
            printf("%s\n", file_name);
        }
        if(entry->d_type == DT_DIR){
            traverse_dir(opendir(file_name), file_name, pstring);
        }
    }
    
    closedir(dir);
    return EXIT_SUCCESS;
}
