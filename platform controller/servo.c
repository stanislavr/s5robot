#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "TA_Header_W2016.h"  /* my macros and constants */
#include "lcd.h"
#include "servo.h"
#include "timer.h"  // Macros and constants for timer3handler.

static unsigned int servoOnTicks;       // Number of TCNT ticks for PWM on-time
static unsigned char servoEdge;         // Set the initial state to be a rising edge
static unsigned int servoLastOnTicks;   // Holds the last used value of onTime

//;**************************************************************
//;*                 configureServo(void)
//;*  Configures the timer module with parameters for PWM operation
//;**************************************************************   
void configureServo(void) {
  LCDputs("Configuring servo.");
  DisableInterrupts;
  TC2 = TCNT + (servoPeriod);	// Preset TC2 for first OC event
  TIOS |= TIOS_IOS2_MASK;      	// Enable TC0 as OC
  TIE |= TIOS_IOS2_MASK;       	// Enable interrupts for TC0

  FORCE_OC_ACTION_NOW(SERVO1,OC_GO_LO);  // Set initial state to low.
  SET_OC_ACTION(SERVO1,OC_TOGGLE);     // Set TC0 to toggle the port pin.
  servoEdge = servoRiseEdge;  // Set the initial state to be a rising edge
  EnableInterrupts;
  setServoPosition(servoLimit_Home);   // Home the servo
}//end of configureServo


//;**************************************************************
//;*                 timer2handler (timer channel 2 handler)
//;*  If rising edge, add on time to channel 2 counter
//;*  If falling edge, add period - on time to channel 2 counter
//;*  Toggle global variable servo2edge for next pass
//;**************************************************************   
interrupt 10 void timer2handler(void) {  
  if(servoEdge == servoRiseEdge) {   // If channel is set to generate a rising edge
    servoLastOnTicks = servoOnTicks;
    TC2 += servoLastOnTicks;         // Set up timer compare for on-time
    servoEdge = servoFallEdge;       // Set channel to generate a falling edge
  }
  else {
    TC2 += (servoPeriod - servoLastOnTicks);  // Set up timer compare channel for off-time
    servoEdge = servoRiseEdge;                  // Set up channel to generate a rising edge
  }
}//end of timer2handler

//;**************************************************************
//;*                 setServoADC
//;*	Converts 8-bit ADC reading to an onTime in TCNT ticks
//;*	Sets number of TCNT ticks to servoOnTime
//;**************************************************************   
void setServoADC(unsigned char ADCreading) {
  DisableInterrupts;  // Prevent the OC interrupt from reading
  servoOnTicks = (ad2ticksFactor*(ADCreading) + ticksOffset);   // Convert from A/D reading to a number of ticks
  EnableInterrupts;
}//end of setServoOnTimeADC

//;**************************************************************
//;*                 setServoPosition
//;*	Converts from degrees to TCNT ticks
//;*	Sets number of TCNT ticks to servoOnTime
//;*	Input: position in degrees, from 0 - 180
//;**************************************************************   
void setServoPosition(unsigned char degrees) {
  DisableInterrupts;  // Prevent the OC interrupt from reading
  
  if(degrees < servoLimit_Lower)
  {
    degrees = servoLimit_Lower;
  }

  servoOnTicks = (degTicksFactor*(degrees) + ticksOffset);   // Convert from A/D reading to a number of ticks
  EnableInterrupts;
  LCDprintf("Servo Set to:\n%i degrees.", degrees);
}//end of setServoPosition