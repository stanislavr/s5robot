/*
 * client.c
 *
 * This is a sample internet client application that will talk
 * to the server s.c via port 5000
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

#define PORT 5000
//#define HOST_NAME 1

int
main (int argc, char *argv[])
{
	char buffer[BUFSIZ];	// data buffer for communications
	int client_socket, len;
	int addr;
	struct sockaddr_in server_addr;
	struct hostent *host;

	// Stan's Raspberry IP to be used by default
 	char server_ip[] = "169.254.168.174"; 

	/*
	 * check for sanity
	 */

#ifdef HOST_NAME
	 if (argc != 2) {
		printf ("Argv[1] aka Host Name Argument wasn't specified!\n");
		return 1;
	}	/* endif */
#else
	if (argc != 2) {
			printf ("Argv[1] aka IP address wasn't specified! %s will be used!\n", server_ip);
			//return 1;
		}	/* endif */
	else {
		 strcpy(server_ip, argv[1]); //replace default ip with given
		}

#endif


#ifdef HOST_NAME
	/*
	 * determine host info for server name supplied
	 */
	if ((host = gethostbyname (argv[1])) == NULL) {
		printf ("grrr, can't get host info!\n");
		return 2;
	}	/* endif */
	memcpy (&addr, host->h_addr, host->h_length);
#else
	addr = inet_addr(server_ip); //convert given ip into int format
#endif


	/*
	 * get a socket for communications
	 */

	if ((client_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		printf ("grrr, can't get a client socket!\n");
		return 3;
	}	/* endif */


	/*
	 * initialize struct to get a socket to host
	 */

	memset (&server_addr, 0, sizeof (server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = addr;
	server_addr.sin_port = htons (PORT);


	printf("Connecting to server %s ... ", server_ip);

	/*
	 * attempt a connection to server
	 */

	if (connect (client_socket, (struct sockaddr *)&server_addr,
	sizeof (server_addr)) < 0) {
		printf ("grrr, can't connet to server!\n");
		close (client_socket);
		return 4;
	}	/* endif */

	printf("Success!\n");

	/*
	 * now that we have a connection, get a commandline from
	 * the user, and fire it off to the server
	 */

	printf ("Enter a command [date | who | df ]: ");
	fflush (stdout);
	fgets (buffer, sizeof (buffer), stdin);
	if (buffer[strlen (buffer) - 1] == '\n')
		buffer[strlen (buffer) - 1] = '\0';

	write (client_socket, buffer, strlen (buffer));

	len = read (client_socket, buffer, sizeof (buffer));

	printf ("Result of command:\n%s\n\n", buffer);

	/*
	 * cleanup
	 */

	close (client_socket);
	printf ("Client is finished\n");

	return 0;
}	/* end main */



