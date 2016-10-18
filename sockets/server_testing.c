/*
 * server.c
 *
 * Internet server application that will respond
 * to requests 

 * credits to Peter Roeser
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

//#define PORT 5000


int
main (int argc, char *argv[])
{
	char buffer[BUFSIZ];	// Data buffer for communications

	int server_socket, client_socket;
	int client_len;
	struct sockaddr_in client_addr, server_addr;
	int len, input;
	FILE *p;
 	char server_port[] = "5000";

	if(argc != 2) {
		printf ("Argv[1] aka Port number wasn't specified!\n%s will be used!\n", server_port);
		//return 1;
		}	/* endif */
	else {
			printf ("Port: %s will be used!\n", argv[1]);
			strcpy(server_port, argv[1]); //replace default ip with given
		}

	/*
	 * obtain a socket for the server
	 */

	if ((server_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		printf ("grrr, can't get the server socket\n");
		return 1;
	}	/* endif */

	/*
	 * initialize our server address info for binding purposes
	 */

	memset (&server_addr, 0, sizeof (server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	//printf("%i", INADDR_ANY);

	server_addr.sin_port = htons (atoi(server_port)); // convert string port # into int

	if (bind (server_socket, (struct sockaddr *)&server_addr, 
	sizeof (server_addr)) < 0) {
		printf ("grrr, can't bind server socket\n");
		close (server_socket);
		return 2;
	}	/* endif */

	printf("Server was successfully started! Now waiting for the clients... \n");

	/*
	 * start listening on the socket
	 */

	if (listen (server_socket, 5) < 0) {
		printf ("grrr, can't listen on socket\n");
		close (server_socket);
		return 3;
	}	/* endif */

	client_len = sizeof (client_addr);
	if ((client_socket = accept (server_socket, 
	(struct sockaddr *)&client_addr, &client_len)) < 0) {
		printf ("grrr, can't accept a packet from client\n");
		close (server_socket);
		return 4;
	}	/* endif */


	// For testing purposes just send whatever characters we read
	while(1) {
		printf("Ready!  Enter what you wanna send\n");

		// get input from user
		fflush(stdin);
		len = scanf("%s", buffer);
		//sprintf(buffer, "%d", input);
		
		// write to the socket
		len = write(client_socket, &buffer, len);	
		printf("writing: %s to socket\n", buffer);		
		if(len < 0) { 
			perror("error: no data sent");
			return -1;
		}
	}

	close(client_socket);
	return 0;
}