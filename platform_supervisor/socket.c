/*
 * socket.c
 *
 *  Created on: Oct 18, 2016
 *      Author: Thomas
 *      References: https://www.cmrr.umn.edu/~strupp/serial.html
 *      			^^ Slightly modifed code found on this site for comms
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
#include <signal.h>
#include <sys/wait.h>

#include "socket.h"

// Function to open the server side socket
int open_socket_server(int PORT) {
	int iSetOption = 1;
	int server_socket;
	struct sockaddr_in server_addr;

	// Obtain a socket for the server
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(server_socket < 0) {
		perror("Could not open server socket.");
		return -1;
	}

	// Set SO_REUSEADDR socket option so we can re-start the program right away
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));

	// Initialize our server address info for binding purposes
	memset (&server_addr, 0, sizeof (server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	server_addr.sin_port = htons (PORT); // convert string port # into int

	if (bind (server_socket, (struct sockaddr *)&server_addr, 
	sizeof (server_addr)) < 0) {
		perror("Could not bind server socket.");
		close (server_socket);
		return -1;
	}

	// Start listening on the socket
	if (listen (server_socket, 5) < 0) {
		perror("Could not listen on the socket.");
		close (server_socket);
		return -1;
	}

	return server_socket;
}

// Function to open the client side socket used for writing data to client
int open_socket_client(int PORT, int server_socket) {
	int client_len;
	int client_socket;
	struct sockaddr_in client_addr;
	
	struct timeval tv;
	tv.tv_sec = 0;	// 1 second timeout for reads
	tv.tv_usec = 500000;	// Do not initialize this, it can cause strange errors	

	// Try to accept a packet from a client to get its address
	printf("Waiting for platform link connection.\n");
	client_len = sizeof (client_addr);
	if ((client_socket = accept (server_socket, 
	(struct sockaddr *)&client_addr, &client_len)) < 0) {
		perror("Could not accept a packet from client.");
		close (server_socket);
		return -1;
	}

	// Set read timeout for the socket
	setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));

	return client_socket;
}