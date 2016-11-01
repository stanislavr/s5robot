/* FileName: heartbeat.c
 * Purpose: 
 * Created on: April 16, 2016
 * Author: David Eelman
 */

/* Includes */
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Signal Linking */
int stopComSig = SIGUSR1;
int heartbeatGoneSig = SIGUSR2;
int killHeartbeatSig = SIGTERM;

/* Function Prototype */
void killHeartbeatHandler(int);
void stopComHandler(int);

/* Flags */
static volatile int stopComFlag = 0;

char HB = 0x48;	// Heartbeat to/from robot

/*------------------------------- MAIN ----------------------------*/
int main(int argc, char* argv[]) {
	pid_t mainProcessPid = atoi(argv[1]);
	int client_socket = atoi(argv[2]);
	unsigned char writeToPort[3] = "<H>";
	int bytesWritten = 0;
	int bytesRead = 0; 
	unsigned char readFromPort[10];
	int attempt = 0;

	signal(killHeartbeatSig, killHeartbeatHandler);
	signal(stopComSig, stopComHandler);

//	printf("Heartbeat process Started!\n"); fflush(stdout);
	for(;;){
		while(!stopComFlag){

			// Try up to three reads (0.5 second timeout)
			while(attempt < 10) {
				// Write HB
				bytesWritten = write(client_socket, writeToPort, sizeof(writeToPort));
				
				// Listen for response (0.5 second timeout)
				memset(&readFromPort, 0, 10);
				bytesRead = read(client_socket, &readFromPort, 3);
				
				// If we got a response, all good in the hood
				if(bytesRead > 0) {
					break;
				}

				// If we need to shut up then STFU and pretend like it's all good
				if(stopComFlag) {
					bytesRead = 1;
					readFromPort[0] = HB;
				}

				attempt ++;
			}

			if(bytesRead <= 0) {
				printf("Failed to read HB response.\n");
				kill(mainProcessPid, heartbeatGoneSig);	// Send signal to UI process to kill everything
			}
			if(readFromPort[0] != HB){
				printf("readFromPort[0] %i\n", readFromPort[0]);
				printf("HB: %i\n", HB);
				printf("%d %s %s", bytesRead, writeToPort, readFromPort); fflush(stdout);
				kill(mainProcessPid, heartbeatGoneSig);	// Send signal to UI process to kill everything
			}
			//printf("received HB\n");
			sleep(1);
			//printf("Success.\n"); fflush(stdout);
		}
	}
}

// Signal handler for when we want to kill this process
void killHeartbeatHandler(int sig){
	exit(0);
}

// Signal handler for when the main process needs control of RS232
void stopComHandler(int sig){
//	stopComFlag == 0 ? 1 : 0;
	if (stopComFlag == 0){
		stopComFlag = 1;
		//printf("Not talking\n");
	}
	else{
		stopComFlag = 0;
		//printf("Talking\n");
	}
	signal(stopComSig, stopComHandler);
}
