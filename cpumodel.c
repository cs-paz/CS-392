/*******************************************************************************
 * Name        : cpumodel.c
 * Author      : Christian Szablewski-Paz & Michael Zylka
 * Date        : 03/26/21
 * Description : Returns this device's processor model name
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool starts_with(const char *str, const char *prefix) {
    /* TODO:
       Return true if the string starts with prefix, false otherwise.
       Note that prefix might be longer than the string itself.
     */
     for(int i = 0; i < strlen(prefix); i++) {
      if(i == strlen(str)) { 
        return false;
      }
      if(prefix[i] != str[i]) {
        return false;
      }
    }
    return true;
}

int main() {
    /* TODO:
       Open "cat /proc/cpuinfo" for reading, redirecting errors to /dev/null.
       If it fails, print the string "Error: popen() failed. %s.\n", where
       %s is strerror(errno) and return EXIT_FAILURE.
     */
    FILE *pipe;
    int status;
    char buf[256];

    if((pipe = popen("cat /proc/cpuinfo", "r")) == NULL){
      fprintf(stderr, "Error: popen() failed. %s.\n", strerror(errno));
      return EXIT_FAILURE;
    }


    /* TODO:
       Allocate an array of 256 characters on the stack.
       Use fgets to read line by line.
       If the line begins with "model name", print everything that comes after
       ": ".
       For example, with the line:
       model name      : AMD Ryzen 9 3900X 12-Core Processor
       print
       AMD Ryzen 9 3900X 12-Core Processor
       including the new line character.
       After you've printed it once, break the loop.
     */
      while(fgets(buf, sizeof(buf), pipe) != NULL) {
      	if(starts_with(buf, "model name")){
        	bool info = false;
        	for(int i = 0; i < strlen(buf); i++) {
          		if(info) {
            		printf("%c", buf[i]);
          		}
          		if(buf[i] == ':') {
          			i++;
            		info = true;
          		}
        	}
        	break;
      	}
  	  }


    /* TODO:
       Close the file descriptor and check the status.
       If closing the descriptor fails, print the string
       "Error: pclose() failed. %s.\n", where %s is strerror(errno) and return
       EXIT_FAILURE.
     */
     
    if((status = pclose(pipe)) < 0) {
      fprintf(stderr, "Error: pclose() failed. %s.\n", strerror(errno));
      return EXIT_FAILURE;
    }


    return !(WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS);
}
