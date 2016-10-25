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
#include "client_socket.h"

// Global variables
int client_socket;				// currently open socket reference
int serial_port;						// currently open serial port reference

char buffer[BUFSIZ];			// data buffer
int bRead;						// Number of bytes read
int bWrite;						// Number of bytes written

int main() {
	// Attempt to set static IP address
	setuid(0);
	if(system("/bin/bash ./setIP.sh")){
		printf("Error: Could not set static IP address.\n");
		return -1;
	}
		
	// Attempt to enable write access for ttyUSB0 (for USB to Serial adapter)
	if(system("/bin/bash ./chmodUSB0.sh")) {
		printf("Error: Could not set permissions for ttyUSB0.\n");
		return -1;
	}

	// Attempt to start webcam service
	//needs to be a fork
	//if(system("/bin/bash ./webcam_start.sh")) {
	//	printf("Error: Could not start webcam service\n");
	//}

	// Set up socket comm.
	if (configure_client_socket()) {
		return -1;
	}

	// Set up serial comm.
	
	// Open serial port
	serial_port = open_port();
	if(serial_port < 0) {
		return -1;
	}
	
	// Configure the port parameters
	char serial_portBuf[10] = {0};
	sprintf(serial_portBuf, "%d", serial_port);
	if (configure_port(serial_port)) {
		return -1;
	}

	// Infinite loop of read/write sweet gateway action
	while(1){

		// Clear buffer
		memset(&buffer, 0, BUFSIZ);

		// Read serial port
		bRead = read(serial_port, &buffer, BUFSIZ);
		if(bRead < 0){
			perror("Serial read failed.\n");
			return -1;
		}

		// Forward serial data to socket if any
		if(bRead) {
			// Write the buffer to the socket
			printf("%s\n", buffer);			
			bWrite = write(client_socket, &buffer, bRead);
			if(bWrite < bRead) {
				perror("Socket write failed.\n");

				// Set up socket comm.
				if (configure_client_socket()) {
					return -1;
				}
			}
		}

		// Clear buffer
		memset(&buffer, 0, BUFSIZ);

		// Read socket
		bRead = read(client_socket, &buffer, BUFSIZ);
		if(bRead < 0){
			perror("Socket read failed.\n");
			
			// Set up socket comm.
			if (configure_client_socket()) {
				return -1;
			}
		}

		// Forward socket data to serial if any
		if(bRead) {
			// Write the buffer to the serial
			printf("%s\n", buffer);
			bWrite = write(serial_port, &buffer, bRead);
			if(bWrite < bRead) {
				perror("Serial write failed.\n");
				return -1;
			}
		}
	}// End of infinite while loop

	// If we ever get here, shits fucked up
	close(serial_port);			// Close the serial port
	close(client_socket);	// Close the socket
	printf("SOMETHING IS DEAD\n");
}
