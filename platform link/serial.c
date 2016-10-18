/*
 * serial.c
 *
 *  Created on: October 17, 2016
 *      Author: Thomas Abdallah
 *		Purpose: All functions for RS232 communication
 *      References: https://www.cmrr.umn.edu/~strupp/serial.html
 *      			^^ Slightly modifed code found on this site for comms
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>	
#include <unistd.h>
#include <termios.h>

 #include "serial.h"

/*
 * open_port()
 *
 *	Inputs: None
 *	Outputs: File descriptor for the open port (if successful)
 */
 int open_port() {
	int fd;	// File descriptor for the serial port

	fd = open(port_serial, O_RDWR | O_NOCTTY);
	if (fd == -1) {
		perror("Could not open port.");
	}
	fcntl(fd, F_SETFL, 0);
	return (fd);
}


/*
 * configure_port()
 *
 *	Inputs: File descriptor for the port to be configured
 *	Outputs: None, errors if it fails
 */
int configure_port(int port) {
	struct termios tty;	
	memset (&tty, 0, sizeof tty);

	// Set Baud Rate 
	cfsetospeed (&tty, (speed_t)B9600);
	cfsetispeed (&tty, (speed_t)B9600);

	// Setting other Port Stuff, Make 8n1
	tty.c_cflag     &=  ~PARENB;            //no parity
	tty.c_cflag     &=  ~CSTOPB;		//one stop bit,
	tty.c_cflag     &=  ~CSIZE;		//clear current size
	tty.c_cflag     |=  CS8;		//set 8 bits
	tty.c_cflag     &=  ~CRTSCTS;           // no flow control ie. no RTS/CTS
	tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

	tty.c_cc[VMIN]   =  0;                  // read doesn't block, instantly available
	tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
	tty.c_lflag &= ~(ICANON | ECHO | ISIG);

	if(tcsetattr(port, TCSANOW, &tty) < 0){
    	fprintf(stderr, "failed to set attr: %d, %s\n", port, strerror(errno));
    	return -1;
	}

	if ( tcsetattr ( port, TCSANOW, &tty ) != 0) {
   		printf("Error from tcgetattr: %s\n", strerror(errno));
   		return -1;
	}
	return 0;
}