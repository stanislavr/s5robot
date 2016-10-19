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

	signal(killHeartbeatSig, killHeartbeatHandler);
	signal(stopComSig, stopComHandler);

//	printf("Heartbeat process Started!\n"); fflush(stdout);
	for(;;){
		while(!stopComFlag){
			bytesWritten = write(client_socket, writeToPort, sizeof(writeToPort));
			
			readFromPort[0] = 0;
			bytesRead = read(client_socket, &readFromPort, 3);
			if(bytesRead <= 0) {
				readFromPort[0] = 0;
				bytesRead = read(client_socket, &readFromPort, 3);
			}
			if(readFromPort[0] != HB){
				//printf("readFromPort[0] %i\n", readFromPort[0]);
				//printf("HB: %i\n", HB);
				//printf("%d %s %s", bytesRead, writeToPort, readFromPort); fflush(stdout);
				kill(mainProcessPid, heartbeatGoneSig);	// Send signal to UI process to kill everything
			}
			sleep(1);
			//printf("Success.\n"); fflush(stdout);
		}
	}
}

// Signal handler for when we want to kill this process
void killHeartbeatHandler(int sig){
//	printf("I got here.\n");
	exit(0);
}

// Signal handler for when the main process needs control of RS232
void stopComHandler(int sig){
//	stopComFlag == 0 ? 1 : 0;
	if (stopComFlag == 0){
		stopComFlag = 1;
	}
	else{
		stopComFlag = 0;
	}
	signal(stopComSig, stopComHandler);
}
