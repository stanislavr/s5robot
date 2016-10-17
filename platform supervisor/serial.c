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
 #include "platform_supervisor.h"

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
void configure_port(int port) {
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
	}

	if ( tcsetattr ( port, TCSANOW, &tty ) != 0) {
   		printf("Error from tcgetattr: %s\n", strerror(errno));
	}
}


/*
 * cmd_send()
 *
 *	Inputs: 
 * 			buffer: Buffer of data which is the command to be send
 *			port_serial: file descriptor for the port to write to
 *	Outputs: Non-zero for failure
 */
int cmd_send(int port, char* buffer) {
	char NAK = 0x49;	// Negative ack from robot
	int bWritten = 0;	// Bytes written to serial port
	int bRead = 0;		// Bytes read from serial port
	char RxBuf[2] = {0};	// Buffer for Rx data (should be ack or nak + cmd)

	// Send SIGUSR1 to Heartbeat proccess to tell it to shut up for a bit
	kill(heartbeatPid, sigUItoHB);	// Send signal to HB process to toggle comms

	bWritten = write(port, buffer, strlen(buffer));
	if (bWritten < strlen(buffer)) {
		printf("Write failed.\n");
		return -1;
	}

	bRead = read(port, &RxBuf, sizeof(RxBuf));
	if(bRead == 0) {
		printf("No response from robot.\n");
		return -1;
	}

	if(RxBuf[0] == NAK) {
		printf("Invalid command received by robot.\n");
		return -1;
	}

	// Send SIGUSR1 to Heartbeat process to tell it to start talking again
	kill(heartbeatPid, sigUItoHB);	// Send signal to HB process to toggle comms

	return 0;	//Successful command.
}