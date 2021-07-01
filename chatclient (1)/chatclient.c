/*
 * Name: Christian Szablewski-Paz & Zach Schuh
 * I pledge my honor that I have abided by the Stevens Honor Code.
*/
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "util.h"

int client_socket = -1;
char username[MAX_NAME_LEN + 1];
char inbuf[BUFLEN + 1];
char outbuf[MAX_MSG_LEN + 1];

int handle_stdin() {
    int status = get_string(outbuf, MAX_MSG_LEN + 1);
    if(status == TOO_LONG) {
        printf("Sorry, limit your message to %d characters.\n", MAX_MSG_LEN);
    }
    else {
        if(send(client_socket, outbuf, strlen(outbuf), 0) < 0) {
            fprintf(stderr, "Error: Failed to send message. %s.\n", strerror(errno));
        }
        if (strcmp(outbuf, "bye") == 0) {
	    	printf("Goodbye.\n");
	    	return EXIT_FAILURE;
	    }
    }
    return EXIT_SUCCESS;
}

int handle_client() {
	int bytes_received;
    if((bytes_received = recv(client_socket, inbuf, BUFLEN - 1, 0)) < 0) {
        fprintf(stderr, "Warning: Failed to receive incoming message. %s.\n", strerror(errno));
    }
    inbuf[bytes_received] = '\0';
    if(bytes_received == 0) {
        fprintf(stderr, "\nConnection to server has been lost.\n");
        return EXIT_FAILURE;
    } 
    else if (strcmp(inbuf, "bye") == 0) {
    	printf("\nServer initiated shutdown.\n");
    	return EXIT_FAILURE;
    } 
    else {
    	inbuf[bytes_received] = '\0';
    	printf("\n%s\n", inbuf);
    }
    return EXIT_SUCCESS;
}

int main(int argc, char ** argv){

	if(argc != 3){
        fprintf(stderr, "Usage: %s <server IP> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
	int ip_conversion;
	struct sockaddr_in server_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	memset(&server_addr, 0, addrlen);
	
	if((ip_conversion = inet_pton(AF_INET, argv[1], &server_addr.sin_addr)) <= 0) {
		fprintf(stderr, "Error: Invalid IP address '%s'.\n", argv[1]);
        return EXIT_FAILURE;
	}
	
	if(!is_integer(argv[2])){
        fprintf(stderr, "Error: Invalid input '%s' received for port number.\n", argv[2]);
        return EXIT_FAILURE;
    }
    
    int port = 0;
    if(!parse_int(argv[2], &port, "port")) {
    	fprintf(stderr, "Error: Integer overflow for %s.\n", argv[2]);
        return EXIT_FAILURE;
    }
    if(port < 1024 || port > 65535){
        fprintf(stderr, "Error: Port must be in range [1024, 65535].\n");
        return EXIT_FAILURE;
    }
    
    server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	
	printf("Username: ");
	fflush(stdout);
	int status;
	
	while ((status = get_string(username, MAX_NAME_LEN + 1)) != OK) { 
		if (status == TOO_LONG) {
			printf("Sorry, limit your username to %d characters.\n", MAX_NAME_LEN);
		}
		printf("Username: ");
    }
    
    printf("Hello, %s. Let's try to connect to the server.\n", username);
    fflush(stdout);
    
    int retval;
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    	fprintf(stderr, "Error: Failed to create socket. %s.\n", strerror(errno));
    	retval = EXIT_FAILURE;
    	return EXIT_FAILURE;
	}

	if (connect(client_socket, (struct sockaddr *)&server_addr, addrlen) < 0) {
        fprintf(stderr, "Error: Failed to connect to server. %s.\n", strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }
    
    int bytes_received;
    if ((bytes_received = recv(client_socket, inbuf, BUFLEN - 1, 0)) < 0) {
        fprintf(stderr, "Error: Failed to receive message from server. %s.\n", strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }
    if (bytes_received == 0) {
        fprintf(stderr, "All connections are busy. Try again later.\n");
        retval = EXIT_FAILURE;
        goto EXIT;
    }
    
    inbuf[bytes_received] = '\0';
    printf("\n%s\n\n", inbuf);
    fflush(stdout);
    
     if (send(client_socket, username, strlen(username), 0) < 0) {
        fprintf(stderr, "Error: Failed to send username to server. %s.\n",
                strerror(errno));
        retval = EXIT_FAILURE;
        goto EXIT;
    }

    
    printf("[%s]: ", username);
    fflush(stdout);
    
    fd_set sockset;
    
    while(true){
    	FD_ZERO(&sockset);
    	FD_SET(STDIN_FILENO, &sockset);
    	FD_SET(client_socket, &sockset);
        
        if(select(client_socket+1, &sockset, NULL, NULL, NULL) < 0){
            fprintf(stderr, "Error: Select Failed.\n");
            goto EXIT;
        }
		
		int result = 0;
        if(FD_ISSET(STDIN_FILENO, &sockset)){
            result = handle_stdin();
        }
        if(FD_ISSET(client_socket, &sockset)){
            result = handle_client();
        }
        
        if(result == 1) {
        	retval = EXIT_SUCCESS;
    		goto EXIT;
        }
        if(result == 2) {
        	retval = EXIT_FAILURE;
    		goto EXIT;
        }
        
        printf("[%s]: ", username);
    	fflush(stdout);
    }
    
    
	EXIT:
    	if (fcntl(client_socket, F_GETFD) >= 0) {
        	close(client_socket);
    	}
    	return retval;    
}
