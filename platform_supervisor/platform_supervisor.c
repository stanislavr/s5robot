/*
 * platform_supervisor.c
 *
 *  Created on: Apr 14, 2016
 *      Author: thomas
 *      References: https://www.cmrr.umn.edu/~strupp/serial.html
 *      			^^ Slightly modifed code found on this site for comms
 */

#include "socket.h"
#include "platform_supervisor.h"
#include "control_keys.h"

char HB = 0x48;	// Heartbeat to/from robot

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
int temperature = 0;		// Robot reported temperature in degrees celcius
int attempts = 0;			// Counter for number of read attempts

int main() {
	// Attempt to set static IP address
	setuid(0);
	if(system("/bin/bash ./setIP.sh")) {
		printf("Error: Could not set static IP address.\n");
		return -1;
	}

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
		return -1;
	}
	else if(heartbeatPid == 0){
		execlp("./heartbeat", "heartbeat", mainProcessPidBuf, client_socket_buf, NULL);
		printf("Heartbeat execlp failed"); fflush(stdout);
		return -1;
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
		kill_everything();
		return 0;
	}

	kill_everything();
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
	printf("-- 2: Set robot speed              --\n");
	printf("-- 3: Pan Camera                   --\n");
	printf("-- 4: Tilt Camera                  --\n");
	printf("-- 5: Drive robot                  --\n");
	printf("-- 6: Get environmental data       --\n");
	printf("-- 7: Exit RobotLAND               --\n");
	printf("-------------------------------------\n");
	printf("------ Select an Action (1-7) -------\n");

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
	// user selected configure modules & home camera
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

	// user selected set DC motor speed
	else if(option == 2) {
		int speed;
		char cmd3[6];

		// get robot speed from user 
		printf("Enter desired robot speed (0-255 mm/s).\n");
		scanf("%i", &speed);
		if(speed < 0) {
			speed = 0;
		}
		if(speed > 255) {
			speed = 255;
		}
	
		// pack up the command and sent it out. 
		sprintf(cmd3, "<S%03d>", speed);

		if(cmd_send(client_socket,cmd3)) {
			return -1;
		}

		printf("Robot speed command processed.\n");
		return 0;		
	}

	// user selected DC motor control
	/*
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

		// get left speed parameter from user
		printf("Enter left motor speed (0-100).\n");
		scanf("%i", &lSpeedNum);
		if(lSpeedNum < 0) {
			lSpeedNum = 0;
		}
		if(lSpeedNum > 100) {
			lSpeedNum = 100;
		}
		sprintf(lSpeed, "%03d", lSpeedNum);

		// get left motor direction parameter from user 
		printf("Enter left motor direction.\n");
		printf("0 = Stop, 1 = Forward, 2 = Reverse\n");
		scanf("%i", &lDirNum);
		if((lDirNum < 0) | (lDirNum > 2)) {
			lDirNum = 0;
		}
		sprintf(&lDir, "%d", lDirNum);
		
		// get right speed parameter from user 
		printf("Enter right motor speed (0-100).\n");
		scanf("%i", &rSpeedNum);
		if(rSpeedNum < 0) {
			rSpeedNum = 0;
		}
		if(rSpeedNum > 100) {
			rSpeedNum = 100;
		}
		sprintf(rSpeed, "%03d", rSpeedNum);		

		// get right motor direction parameter from user 
		printf("Enter right motor direction.\n");
		printf("0 = Stop, 1 = Forward, 2 = Reverse\n");
		scanf("%i", &rDirNum);
		if((rDirNum < 0) | (rDirNum > 2)) {
			rDirNum = 0;
		}
		sprintf(&rDir, "%d", rDirNum);

		// Ship out the command like a boss 
		printf("Sending DC motor command.\n");
		sprintf(cmd2, "<B%s%c%s%c>", (char*)lSpeed, lDir, (char*)rSpeed, rDir);

		if(cmd_send(client_socket,cmd2)) {
			return -1;
		}

		printf("DC command processed.\n");
		return 0;
	}*/

	// user selected pan camera 
	else if(option == 3)
	{
		int position;
		char cmd3[6];

		// get pan position from user 
		printf("Enter position to pan camera (degrees).\n");
		printf("0 = Left Limit\n180 = Right Limit\n");
		scanf("%i", &position);
		if(position < 0) {
			position = 0;
		}
		if(position > 180) {
			position = 180;
		}
	
		// pack up the command and sent it out. 
		sprintf(cmd3, "<C%03d>", position);

		if(cmd_send(client_socket,cmd3)) {
			return -1;
		}

		printf("Camera pan processed.\n");
		return 0;
	}

	// user selected tilt camera 
	else if(option == 4)
	{
		int position;
		char cmd4[6];

		// get tilt position from user 
		printf("Enter position to tilt camera (degrees).\n");
		printf("0 = Upper Limit\n180 = Lower Limit\n");
		scanf("%i", &position);
		if(position < 0) {
			position = 0;
		}
		if(position > 180) {
			position = 180;
		}
	
		// pack up the command and sent it out. 
		sprintf(cmd4, "<D%03d>", position);

		if(cmd_send(client_socket,cmd4)) {
			return -1;
		}
		return 0;
	}

	// user selected drive robot
	else if(option == 5)
	{
		printf("Entering driving mode.\n");
		
		if(acceptArrowKey(client_socket)) {
			printf("acceptArrowKey error.\n");
			return -1;
		}
		
		return 0;
	}

	// user selected get environmental data
	else if(option == 6)
	{
		int bRead = 0;
		unsigned char readBuf;
		float temperature;
		char attempts = 0;			// Counter for number of read attempts

		printf("Getting environmental data.\n");
		char cmd1[] = "<E>";
		
		// Send SIGUSR1 to Heartbeat proccess to tell it to shut up for a bit
		if(kill(heartbeatPid, sigUItoHB)) {
			printf("Error: Could not shut up the HB process.\n");
		}

		// Send the request for data
		if(cmd_send(client_socket,cmd1)) {
			return -1;
		}

		// Read data back - try up to 10 times then fail.
		while(attempts < 10) {
			bRead = read(client_socket, &readBuf, 1);

			// If we read data, check that it isn't a heartbeat
			if(bRead > 0) {
				// If it is a heartbeat just ignore it
				if(readBuf == HB) {
					attempts ++;
					continue;
				}
				else {
					break;
				}
			}

			attempts ++;
		}

		// If we didn't read anything exit with error.
		if(bRead <=0) {
			printf("Failed to get environmental data.\n");
			return -1;
		}

		// If we got data back, it is probably the temperature...
		temperature = readBuf / 10.0;
		//printf("Temperature: %i celcius.\n", readBuf);
		printf("Temperature: %f celcius.\n", temperature);

		// Send SIGUSR1 to Heartbeat process to tell it to start talking again
		if (kill(heartbeatPid, sigUItoHB)) {
			printf("Error: Could not wake up the HB process.\n");
		}

		return 0;
	}

	// user selected exit program 
	else if(option == 7)
	{
		// exit the program with code 1 
		printf("---------- Exiting Script ---------\n");

		return 1;
	}

	// user entered invalid input 
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
	
	kill_everything();

	printf("Lost Heartbeat.\n");

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

	char NAK = 0x49;	// Invalid response from robot (I)
	char ACK = 0x53;	// Successful response from robot (S)
	int bWritten = 0;	// Bytes written to serial port
	int bRead = 0;		// Bytes read from serial port
	char RxBuf[4] = {0};	// Buffer for Rx data (should be ack or nak + cmd)
	int attempts = 0;		// Try up to 3 times to read response from robot

	// Send SIGUSR1 to Heartbeat proccess to tell it to shut up for a bit
	//if(kill(heartbeatPid, sigUItoHB)) {
	//	printf("Error: Could not shut up the HB process.\n");
	//}

	// Send the command to the robot
	bWritten = write(socket_client, buffer, strlen(buffer));
	if (bWritten < strlen(buffer)) {
		perror("Write failed.");
		return -1;
	}
	
	// Read the response from the robot, make sure it is an ack (S)
	/*
	while(attempts < 3) {
		memset(&RxBuf, 0, 4);
		bRead = read(socket_client, &RxBuf, 4);
		printf("Read: %s\n", RxBuf);

		if(RxBuf[0] == ACK){
			break;
		}
		if(RxBuf[1] == ACK) {
			break;
		}

		attempts ++;
		write(socket_client, buffer, strlen(buffer));
	}

	// If nothing read, throw error
	if(bRead <= 0) {
		printf("Error: No response read from robot.\n");
		return -1;
	}

	// If the response is NAK the robot is confused
	if(RxBuf[0] == NAK) {
		printf("Invalid command received by robot.\n");
		return -1;
	}
	

	// Send SIGUSR1 to Heartbeat process to tell it to start talking again
	if (kill(heartbeatPid, sigUItoHB)) {
		printf("Error: Could not wake up the HB process.\n");
	}
*/

	return 0;	//Successful command.
}

int kill_everything(){
	endwin(); /* End curses mode         */

	close(server_socket);	// Close the server socket
	close(client_socket);	// Close the client socket

	if(system("/bin/bash ./forceKillport.sh")) {
		printf("Error: Could not kill port.\n");
		return -1;
	}

	printf("Port Closed.\n");
	kill(heartbeatPid, SIGTERM);

	return 0;
}