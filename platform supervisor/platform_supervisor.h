/*
 * platform_supervisor.h
 *
 *  Created on: October 17, 2016
 *      Author: Thomas Abdallah
 *		Purpose: Header for platform supervisor
 *      References: https://www.cmrr.umn.edu/~strupp/serial.html
 *      			^^ Slightly modifed code found on this site for comms
 */
#include <signal.h>

extern pid_t heartbeatPid;		// PID of the heartbeat process
extern int sigUItoHB;	// Signal from UI process to HB process to enable/disable HB
extern int sigHBtoUI;	// Signal from HB process to UI process to indicate HB failure