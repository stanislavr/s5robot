/*
 * platform_supervisor.c
 *
 *  Created on: Apr 14, 2016
 *      Author: thomas
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

#include "socket.h"
#include "platform_supervisor.h"

int drawMenu();			// Draw the menu
int doStuff(int option);	// Function to handle menu options, takes option, returns success/fail/exit
void sig1handler(int sig);	// Signal from UI process to HB process to enable/disable HB
void sig2handler(int sig);	// Signal from HB process to UI process to indicate HB failure

pid_t heartbeatPid;		// PID of the heartbeat process
int sigUItoHB = SIGUSR1;	// Signal from UI process to HB process to enable/disable HB
int sigHBtoUI = SIGUSR2;	// Signal from HB process to UI process to indicate HB failure
int server_socket;			// Server side socket ID for reading data from client
int client_socket;			// Client side socket ID for writing data to client
int PORT = 5000;			// Port to use for socket comms

int main() {
	// Set up signal handlers.
	signal(sigHBtoUI, sig2handler);

	// PID for UI and HB processes
	pid_t mainProcessPid = getpid();
	char mainProcessPidBuf[10] = {0};
	sprintf(mainProcessPidBuf, "%d", mainProcessPid);

	// Set up communication
	server_socket = open_socket_server(PORT);
	char server_socket_buf[10] = {0};
	sprintf(server_socket_buf, "%d", server_socket);
	if(server_socket < 0) {
		return -1;
	}

	client_socket = open_socket_client(PORT, server_socket);
	char client_socket_buf[10] = {0};
	sprintf(client_socket_buf, "%d", client_socket);
	if(client_socket < 0) {
		return -1;
	}


	// Wait for the robot heartbeat before doing anything.
	printf("Waiting for robot connection...\n");
	int bRead = 0;
	char readBuf;
	char hbCalling[3] = "<H>";
	while(bRead <= 0) {
		//printf("sending HB\n");
		if(write(client_socket, hbCalling, 3) < 1) {
			perror("Failed to write client socket.");
		}
		bRead = read(client_socket, &readBuf, 1);
	}

	// Set up heartbeat process
	heartbeatPid = fork();
	if(heartbeatPid  < 0){
		printf("Failed to fork hb process."); fflush(stdout);
	}
	else if(heartbeatPid == 0){
		execlp("./heartbeat", "heartbeat", mainProcessPidBuf, client_socket_buf, NULL);
		printf("Heartbeat execlp failed"); fflush(stdout);
	}

	// Create a really great UI
	int option;
	int result = 0;
	while(!result)
	{
		option = drawMenu();
		result = doStuff(option);
	}

	if(result)
	{
		close(server_socket);	// Close the server socket
		close(client_socket);	// Close the client socket
		printf("Port Closed.\n");
		kill(heartbeatPid, SIGTERM);
		return 0;
	}

	close(server_socket);	// Close the server socket
	close(client_socket);	// Close the client socket
	printf("Port Closed.\n");
	kill(heartbeatPid, SIGTERM);
	return 1;	//return 0 for error (shouldn't get here)

}

/* Function to draw the menu
 * Takes no input.
 * Returns the value/option the user entered
 */

int drawMenu()
{
	/* display menu options */
	printf("--------- Welcome to RobotLAND ------\n");
	printf("-- 1: Home camera                  --\n");
	printf("-- 2: Control DC Motors            --\n");
	printf("-- 3: Pan Camera                   --\n");
	printf("-- 4: Tilt Camera                  --\n");
	printf("-- 5: Demo Function                --\n");
	printf("-- 6: Exit RobotLAND               --\n");
	printf("-------------------------------------\n");
	printf("------ Select an Action (1-6) -------\n");

	/* get input from user */
	int input;
	fflush(stdin);
	scanf("%d", &input);
	return input;
}

/* Function to handle menu options
 * Input: option the user selected from the menu, as an int
 * Output: 0 for success, 1 for close program
 */

int doStuff(int option)
{
	/* user selected configure modules & home camera */
	if(option == 1)
	{
		printf("Homing camera.\n");
		char cmd1[] = "<A>";
		
		if(cmd_send(client_socket,cmd1)) {
			return -1;
		}

		printf("Camera homed.\n");

		return 0;
	}

	/* user selected DC motor control */
	else if(option == 2)
	{

		char cmd2[12];	

		int lSpeedNum = 0;
		int rSpeedNum = 0;
		int lDirNum = 0;
		int rDirNum = 0;

		char lSpeed[4];
		char rSpeed[4];
		char lDir = 0;
		char rDir = 0;

		/* get left speed parameter from user */
		printf("Enter left motor speed (0-100).\n");
		scanf("%i", &lSpeedNum);
		if(lSpeedNum < 0) {
			lSpeedNum = 0;
		}
		if(lSpeedNum > 100) {
			lSpeedNum = 100;
		}
		sprintf(lSpeed, "%03d", lSpeedNum);

		/* get left motor direction parameter from user */
		printf("Enter left motor direction.\n");
		printf("0 = Stop, 1 = Forward, 2 = Reverse\n");
		scanf("%i", &lDirNum);
		if((lDirNum < 0) | (lDirNum > 2)) {
			lDirNum = 0;
		}
		sprintf(&lDir, "%d", lDirNum);
		
		/* get right speed parameter from user */
		printf("Enter right motor speed (0-100).\n");
		scanf("%i", &rSpeedNum);
		if(rSpeedNum < 0) {
			rSpeedNum = 0;
		}
		if(rSpeedNum > 100) {
			rSpeedNum = 100;
		}
		sprintf(rSpeed, "%03d", rSpeedNum);		

		/* get right motor direction parameter from user */
		printf("Enter right motor direction.\n");
		printf("0 = Stop, 1 = Forward, 2 = Reverse\n");
		scanf("%i", &rDirNum);
		if((rDirNum < 0) | (rDirNum > 2)) {
			rDirNum = 0;
		}
		sprintf(&rDir, "%d", rDirNum);

		/* Ship out the command like a boss */
		printf("Sending DC motor command.\n");
		sprintf(cmd2, "<B%s%c%s%c>", (char*)lSpeed, lDir, (char*)rSpeed, rDir);

		if(cmd_send(client_socket,cmd2)) {
			return -1;
		}

		printf("DC command processed.\n");
		return 0;
	}

	/* user selected pan camera */
	else if(option == 3)
	{
		int position;
		char cmd3[6];

		/* get pan position from user */
		printf("Enter position to pan camera (degrees).\n");
		printf("0 = Left Limit\n180 = Right Limit\n");
		scanf("%i", &position);
		if(position < 0) {
			position = 0;
		}
		if(position > 180) {
			position = 180;
		}
	
		/* pack up the command and sent it out. */
		sprintf(cmd3, "<C%03d>", position);

		if(cmd_send(client_socket,cmd3)) {
			return -1;
		}

		printf("Camera pan processed.\n");
		return 0;
	}

	/* user selected tilt camera */
	else if(option == 4)
	{
		int position;
		char cmd4[6];

		/* get tilt position from user */
		printf("Enter position to tilt camera (degrees).\n");
		printf("0 = Upper Limit\n180 = Lower Limit\n");
		scanf("%i", &position);
		if(position < 0) {
			position = 0;
		}
		if(position > 180) {
			position = 180;
		}
	
		/* pack up the command and sent it out. */
		sprintf(cmd4, "<D%03d>", position);

		if(cmd_send(client_socket,cmd4)) {
			return -1;
		}
		return 0;
	}

	/* user selected demo function */
	else if(option == 5)
	{
		printf("Sending a heartbeat.\n");
		char cmd5[2];
		sprintf(cmd5, "<H>");

		if(cmd_send(client_socket,cmd5)) {
			return -1;
		}
		
		return 0;
	}

	/* user selected exit program */
	else if(option == 6)
	{
		/* exit the program with code 1 */
		printf("---------- Exiting Script ---------\n");

		return 1;
	}

	/* user entered invalid input */
	else
	{
		printf("Invalid Input. Please enter 1-6 for Option\n");
		return 0;
	}
}

// Signal from UI process to HB process to enable/disable HB
void sig1handler(int sig) {
	signal(sigUItoHB, sig1handler);
}

// Signal from HB process to UI process to indicate HB failure
void sig2handler(int sig) {
	signal(sigHBtoUI, sig2handler);
	printf("Lost Heartbeat.\n");
	kill(heartbeatPid, SIGTERM); // Kill the hb process
	exit(0);
}

/*
 * cmd_send()
 * 
 * Inputs: 
 *			socket_client - id of the socket to use for data transmission
 * 			socket_server - id of the socket to use for data reading (for robot response)
 * 			buffer - the data to be sent
 *
 * Outputs:
 *			Non-zero for error
 */
int cmd_send(int socket_client, char* buffer) {

	char NAK = 0x49;	// Negative ack from robot
	int bWritten = 0;	// Bytes written to serial port
	int bRead = 0;		// Bytes read from serial port
	char RxBuf[2] = {0};	// Buffer for Rx data (should be ack or nak + cmd)

	// Send SIGUSR1 to Heartbeat proccess to tell it to shut up for a bit
	kill(heartbeatPid, sigUItoHB);	// Send signal to HB process to toggle comms

	bWritten = write(socket_client, buffer, strlen(buffer));
	if (bWritten < strlen(buffer)) {
		perror("Write failed.");
		return -1;
	}

	while(bRead <= 0) {
		bRead = read(client_socket, &RxBuf, sizeof(RxBuf));
	}

	if(RxBuf[0] == NAK) {
		printf("Invalid command received by robot.\n");
		return -1;
	}

	// Send SIGUSR1 to Heartbeat process to tell it to start talking again
	kill(heartbeatPid, sigUItoHB);	// Send signal to HB process to toggle comms

	return 0;	//Successful command.
}