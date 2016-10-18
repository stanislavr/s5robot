/*
 * serial.h
 *
 *  Created on: October 17, 2016
 *      Author: Thomas Abdallah
 *		Purpose: All functions for RS232 communication
 *      References: https://www.cmrr.umn.edu/~strupp/serial.html
 *      			^^ Slightly modifed code found on this site for comms
 */

#define port_serial "/dev/ttyUSB0"	// Port to use for RS232

int open_port();				// Open the serial port.
int configure_port(int port);	// Configure a serial port.
int cmd_send(int port, char* buffer);		// Send a command to the robot over serial.