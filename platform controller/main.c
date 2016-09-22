#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <stdio.h>
#include <stdlib.h>
#include "sci.h"
#include "timer.h"
#include "lcd.h"
#include "stepper.h"
#include "servo.h"
#include "dcm.h"

unsigned char greeting[] = "Hello World ";

char speedBuf[3];
char dirBuf;
unsigned char lSpeed;
unsigned char lDir;
unsigned char rSpeed;
unsigned char rDir;
unsigned char camPos;

volatile unsigned char param1;
volatile unsigned char param2;
volatile unsigned char param3;

void main(void) {
  CONFIGURE_5VA;
  ENABLE_5VA;
  CONFIGURE_LEDS;
  LED1_ON;
  LED2_OFF;
  
  configureTimer();
  configureLCD();
  configureDCM();
  configureServo();
  configureHB();
  configureSCI();
  configureStepper();  
  
  LCDprintf("Hello World");
  //putsSCI(greeting);

	EnableInterrupts;
	
  for(;;) {
    switch(consumeSCI()) {
      // If the buffer is empty, nothing to do.
      case bufEmpty:
        break;

      // Case A == Configure all modules
      case 'A':       // Configure all modules
        LCDprintf("Go home\nCamera.");

        // Home servo.
        setServoPosition(servoLimit_Home);       

        // Acknowledge receipt of command.        
        putcSCI(ACK);
        putcSCI('A');
        
        // Home stepper.
        homeStepper();

        LCDprintf("Camera is home.");
        break;
        
      // Case B == Change speed/direction of DC motors
      case 'B':
        LCDprintf("Processing\ndcmParams");
        
        // Read & check first parameter (left motor speed)        
        param1 = consumeSCI();
        param2 = consumeSCI();
        param3 = consumeSCI();
        sprintf(speedBuf, "%c%c%c", param1, param2, param3);
        lSpeed = (char)atoi(speedBuf);
        if(!(0 <= lSpeed <= 100)) {
          putcSCI(NAK); //Invalid speed input
          break;
        }
        
        // Read & check second parameter (left motor direction)
        dirBuf = consumeSCI();
        lDir = (char)atoi(&dirBuf);
        if(!(0 <= lDir <= 2)) {
          putcSCI(NAK); //Invalid direction input
          break;
        }
                 
        // Read & check third parameter (right motor speed)
        param1 = consumeSCI();
        param2 = consumeSCI();
        param3 = consumeSCI();
        sprintf(speedBuf, "%c%c%c", param1, param2, param3);
        rSpeed = (char)atoi(speedBuf);
        if(!(0 <= rSpeed <= 100)) {
          putcSCI(NAK); //Invalid speed input
          break;
        }
        
        // Read & check fourth parameter (right motor direction)
        dirBuf = consumeSCI();
        rDir = (char)atoi(&dirBuf);
        if(!(0 <= rDir <= 2)) {
          putcSCI(NAK); //Invalid direction input
          break;
        }
        
        // If all paramters are valid, set the motors speed & direction
        dcmControl(lSpeed, lDir, dcmLeft);  //Set left motor params
        dcmControl(rSpeed, rDir, dcmRight); //Set right motor params
        
        // Acknowledge receipt of command.        
        putcSCI(ACK);
        putcSCI('B');
        setHBtimer(); // Delay the heartbeat timer again.
        LCDprintf("lSpd:%i lDir:%i\nrSpd:%i rDir:%i", lSpeed, lDir, rSpeed, rDir);
        break;
        
      // Case C == Pan camera to position
      case 'C':
        LCDprintf("Processing\ncamera pan");
        
        // Read & check position parameter
        param1 = consumeSCI();
        param2 = consumeSCI();
        param3 = consumeSCI();
        sprintf(speedBuf, "%c%c%c", param1, param2, param3);
        camPos = (char)atoi(speedBuf);
        if(!(0 <= camPos <= 180)) {
          putcSCI(NAK); //Invalid speed input
          break;
        }
        
        // Acknowledge receipt of command.        
        putcSCI(ACK);
        putcSCI('C');      
        setStepperPos(camPos);
        break;

      // Case D == Tilt camera to position
      case 'D':
        LCDprintf("Processing\ncamera tilt");
        
        // Read & check position parameter
        param1 = consumeSCI();
        param2 = consumeSCI();
        param3 = consumeSCI();
        sprintf(speedBuf, "%c%c%c", param1, param2, param3);
        camPos = (char)atoi(speedBuf);
        if(!(0 <= camPos <= 180)) {
          putcSCI(NAK); //Invalid speed input
          break;
        }
        
        // Acknowledge receipt of command.        
        putcSCI(ACK);
        putcSCI('D');      
        setServoPosition(camPos);
        break;
          
      // Case H == Received a heatbeat, echo a heartbeat.
      /*case 'H':
        TOGGLE_LEDS;
        putcSCI(HB);
        setHBtimer(); // Delay the heartbeat timer again.
        break;
        */

      // Default == Catch all unknown commands        
      default:  // Unknown input.  Return NAK per spec.
        putcSCI(NAK);
    }     
  } /* loop forever */
}