/*
 * platform_link.c
 *
 *  Created on: Oct 17, 2016
 *      Author: David
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
#include <fcntl.h>
#include <errno.h>

#include "serial.h"
#include "platform_link.h"

// Global variables
char serialBuffer[BUFSIZ];	// data buffer for communications
char socketBuffer[BUFSIZ];
int client_socket;
int fdPort;

int main() {
	/* SOCKET CODE */
	//int len;
	
	// Set up socket comm.
	configure_client_socket();	// Error check this later

	// Set up serial comm.
	fdPort = open_port();		// Open the serial port.
	char fdPortBuf[10] = {0};
	sprintf(fdPortBuf, "%d", fdPort);
	configure_port(fdPort);		// Configure port parameters.	

	while(1){
		bReadSerial = read(fdPort, serialBuffer, sizeof(serialBuffer));
		if(bReadSerial > 0){
			//Write to socket
			write (client_socket, serialBuffer, strlen (serialBuffer));
		}
		else if(bReadSerial < 0){
			// Error
		}
		
		bReadSocket = read(client_socket, buffer, sizeof (buffer));
		if(bReadSocket > 0){
			// Write to serial
			bWrittenSerial = write(fdPort, buffer, strlen(buffer));
			if (bWritten < strlen(buffer)) {
				printf("Write failed.\n");
				return -1;
			}
		}
		else if(bReadSocket < 0){
			// Error
		}

	}
		//write(fdPort, hbCalling, 3);
		//bRead = read(fdPort, &readBuf, 1);
    sprintf(cmd1, "<C%03d>", position);
	if(cmd_send(fdPort,cmd1)) {
			return -1;
		}


	close(fdPort);	// Close the serial port
	printf("Port Closed.\n");
	return 1;	//return 0 for error (shouldn't get here)
}





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