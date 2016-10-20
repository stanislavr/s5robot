/*
 * platform_supervisor.h
 *
 *  Created on: October 17, 2016
 *      Author: Thomas Abdallah
 *		Purpose: Header for platform supervisor
 *      References: https://www.cmrr.umn.edu/~strupp/serial.html
 *      			^^ Slightly modifed code found on this site for comms
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

extern pid_t heartbeatPid;		// PID of the heartbeat process
extern int sigUItoHB;	// Signal from UI process to HB process to enable/disable HB
extern int sigHBtoUI;	// Signal from HB process to UI process to indicate HB failure

int cmd_send(int socket_client, char* buffer);	// Send a command to the robot over the socket.
//int cmd_send(int socket_client, int socket_server, char* buffer);	// Send a command to the robot over the socket.