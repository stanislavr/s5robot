/*
 * client_socket.h
 *
 *  Created on: October 17, 2016
 *      Author: David Eelman
 *		Purpose: Header for client socket
 *      References: 
 */

#define server_ip "169.254.168.174"	// Static IP address of platform supervisor - linux VM on school desktop
#define server_port "5000"			// Port for socket comms

extern int client_socket;
int configure_client_socket();