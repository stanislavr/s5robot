/*
 * platform_supervisor.h
 *
 *  Created on: October 17, 2016
 *      Author: Thomas Abdallah
 *		Purpose: Header for platform supervisor
 *      References: https://www.cmrr.umn.edu/~strupp/serial.html
 *      			^^ Slightly modifed code found on this site for comms
 */

extern pid_t heartbeatPid;		// PID of the heartbeat process
extern int sigUItoHB;	// Signal from UI process to HB process to enable/disable HB
extern int sigHBtoUI;	// Signal from HB process to UI process to indicate HB failure

int cmd_send(int socket_client, char* buffer);	// Send a command to the robot over the socket.
//int cmd_send(int socket_client, int socket_server, char* buffer);	// Send a command to the robot over the socket.