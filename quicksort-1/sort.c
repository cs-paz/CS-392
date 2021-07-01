/*******************************************************************************
 * Name        : sort.c
 * Author      : Christian Szablewski-Paz & Zac Schuh
 * Date        : 3/3/21
 * Description : Uses quicksort to sort a file of either ints, doubles, or
 *               strings.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "quicksort.h"

#define MAX_STRLEN     64 // Not including '\0'
#define MAX_ELEMENTS 1024

typedef enum {
    STRING,
    INT,
    DOUBLE
} elem_t;

/**
 * Reads data from filename into an already allocated 2D array of chars.
 * Exits the entire program if the file cannot be opened.
 */
 size_t read_data(char *filename, char **data) {
    // Open the file.
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Cannot open '%s'. %s.\n", filename,
                strerror(errno));
        free(data);
        exit(EXIT_FAILURE);
    }

    // Read in the data.
    size_t index = 0;
    char str[MAX_STRLEN + 2];
    char *eoln;
    while (fgets(str, MAX_STRLEN + 2, fp) != NULL) {
        eoln = strchr(str, '\n');
        if (eoln == NULL) {
            str[MAX_STRLEN] = '\0';
        } else {
            *eoln = '\0';
        }
        // Ignore blank lines.
        if (strlen(str) != 0) {
            data[index] = (char *)malloc((MAX_STRLEN + 1) * sizeof(char));
            strcpy(data[index++], str);
        }
    }

    // Close the file before returning from the function.
    fclose(fp);

    return index;
}

void print_data(void* data, int index, bool dFlag) {
	for (int i = 0; i < index; i++) {
		if(!dFlag){
			printf("%s\n", *((char**)data + i));
		}
		else{
			printf("%.6f\n", strtod(*((char**)data + i), NULL));
		}
    }
}


/**
 * Basic structure of sort.c:
 *
 * Parses args with getopt.
 * Opens input file for reading.
 * Allocates space in a char** for at least MAX_ELEMENTS strings to be stored,
 * where MAX_ELEMENTS is 1024.
 * Reads in the file
 * - For each line, allocates space in each index of the char** to store the
 *   line.
 * Closes the file, after reading in all the lines.
 * Calls quicksort based on type (int, double, string) supplied on the command
 * line.
 * Frees all data.
 * Ensures there are no memory leaks with valgrind. 
 */
 
int main(int argc, char **argv) {
	
	bool iFlag = false;
	bool dFlag = false;
	size_t index = 0;
	
	//char *cvalue = NULL;
	int c;
	
	char **file_data;
	file_data = (char**)malloc((MAX_ELEMENTS * sizeof(char*)));
	
	opterr = 0;
	
	if(argc == 1){
		printf("Usage: ./sort [-i|-d] filename\n\t-i: Specifies the file contains ints.\n\t-d: Specifies the file contains doubles.\n\tfilename: The file to sort.\n\tNo flags defaults to sorting strings.\n");
		return EXIT_FAILURE;
	}

	while((c = getopt(argc, argv, "id:")) != -1) {
		switch(c) {
			case 'i':
				iFlag = true;
				break;
			case 'd':
				dFlag = true;
				break;
			case '?':
				fprintf(stderr, "Error: Unknown option '-%c' received.\n", optopt);
				printf("Usage: ./sort [-i|-d] filename\n\t-i: Specifies the file contains ints.\n\t-d: Specifies the file contains doubles.\n\tfilename: The file to sort.\n\tNo flags defaults to sorting strings.\n");
				return EXIT_FAILURE;
				return 1;
			default:
				abort();
		}
	}
	
	if(iFlag && dFlag){
		fprintf(stderr, "Error: Too many flags specified.\n");
		return EXIT_FAILURE;
	}
	
	if(iFlag || dFlag){
		if(argc > 3){
			fprintf(stderr, "Error: Too many files specified.\n");
			return EXIT_FAILURE;
		}
		else if(argc < 3){
			fprintf(stderr, "Error: No input file specified.\n");
			return EXIT_FAILURE;
		}
		else{
			index = read_data(argv[2], file_data);
		}
	}
	else{
		if(argc > 2){
			fprintf(stderr, "Error: Too many files specified.\n");
			return EXIT_FAILURE;
		}
		else if(argc < 2){
			fprintf(stderr, "Error: No input file specified.\n");
			return EXIT_FAILURE;
		}
		else{
			index = read_data(argv[1], file_data);
		}
	}

	if(iFlag){
		quicksort(file_data, index, sizeof(int), int_cmp);
	}
	else if(dFlag){
		quicksort(file_data, index, sizeof(double), dbl_cmp);
	}
	else{
		quicksort(file_data, index, sizeof(char*), str_cmp);
	}
	
	print_data(file_data, index, dFlag);
	
	for(int i = 0; i < index; i++){
		free(file_data[i]);
	}
	free(file_data);
	
    return EXIT_SUCCESS;
}
