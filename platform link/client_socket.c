/*
 * client_socket.c
 *
 *  Created on: October 17, 2016
 *      Author: David Eelman
 *		Purpose: All functions for Socket communication
 *      References: 
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int configure_client_socket(){
	int addr;
	char server_ip[] = "169.254.168.174"; 
 	char server_port[] = "5000";
	struct sockaddr_in server_addr;

	addr = inet_addr(server_ip); //convert given ip into int format

	// Get a socket for communications
	if ((client_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		printf ("Error, can't get a client socket!\n");
		//return 3;
	}

	// Initialize a struct to get a socket to the server
	memset (&server_addr, 0, sizeof (server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = addr;
	server_addr.sin_port = htons (atoi(server_port)); // convert string port # into int

	printf("Connecting to server...\n");

	// Attempt a connection to server
	if (connect (client_socket, (struct sockaddr *)&server_addr, sizeof (server_addr)) < 0) {
		printf ("Error, can't connect to server!\n");
		close (client_socket);
		//return 4;
	}

	printf("Success!\n");
	return 0;
}