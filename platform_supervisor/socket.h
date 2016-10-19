/*
 * socket.h
 *
 *  Created on: October 18, 2016
 *      Author: Thomas Abdallah
 *		Purpose: Header for socketes
 *      References: https://www.cmrr.umn.edu/~strupp/serial.html
 *      			^^ Slightly modifed code found on this site for comms
 */

int open_socket_server(int PORT);
int open_socket_client(int PORT, int server_socket);