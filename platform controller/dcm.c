#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "TA_Header_W2016.h"  /* my macros and constants */
#include "dcm.h"
#include "timer.h"
#include "lcd.h"

// Global variables to control the DC Motors
unsigned char targetA = 0;	// Target speed for Motor A in mm/s
unsigned char targetB = 0;	// Target speed for Motor B in mm/s
unsigned char dcmA_dir = 0; // Currently set direction for Motor A
unsigned char dcmB_dir = 0; // Currently set direction for Motor B

//;**************************************************************
//;*                 configureDCM(void)
//;*  Configures the lower nibble of Port B for Motor Direction Control
//;*  Sets up PWM module for period of 100 and duty cycle of zero  
//;*
//;**************************************************************   
void configureDCM(void) {
	LCDprintf("Configuring DC\nMotors.");

	// Configure direction ports for DC Motor H-Bridge
	dcmDDR |= ( dcmAdir1 | dcmAdir2 | dcmBdir1 | dcmBdir2 );	// Configure ports as outputs
	CLEAR_BITS(dcmPort, (dcmAdir1 | dcmAdir2 | dcmBdir1 | dcmBdir2));	// Turn off all motors
	
	// Configure parameters for all PWM channels
	PWMCTL = MODE_8BIT;		// Configure PWM hardware for 8-bit mode
	PWMPRCLK = NO_PRESCALE;	// Set PWM Clock A = E-Clock (8MHz)
	PWMSCLA = CLKSA_SCALE;	// Set PWM Clock SA = 1/2 Clock A

	// Configure PWM Channel 4 and 5 for Motor A and Motor B respectively
	dcmPWM_CLK_A;			    // Use clock A for PWM4 and PWM5
	dcmPWM_ACTIVE_HIGH;		// Use active high output for PWM4 and PWM5
	dcmPWM_CENTRE_ALIGNED;	// Use centre aligned mode for PWM4 and PWM5
	dcmPWM_SET_PERIOD_A;		// Set period = 100 for PWM4
	dcmPWM_SET_PERIOD_B;    // Set period = 100 for PWM5
	dcmPWM_SET_DUTY_A(targetA);	// Set duty = 0 to start.
	dcmPWM_SET_DUTY_B(targetB);	// Set duty = 0 to start.
	dcmPWM_CLR_CNT_A;		// Reset counter for PWM4
	dcmPWM_CLR_CNT_B;		// Reset counter for PWM5
	dcmPWM_ENABLE;			// Enable the PWM output

  	//Configure IC for encoders (EncA = PT0, EncB = PT1)
	TIOS &= LOW((~TIOS_IOS0_MASK)); 	// Enable TC0 as IC for EncA
	TIOS &= LOW((~TIOS_IOS1_MASK));	// Enable TC1 as IC for EncB
	TCTL4 = TCTL4_INIT;			// Capture on rising edges of TC0 and TC1
	TIE = (TIOS_IOS0_MASK | TIOS_IOS1_MASK);     // Enable interrupts for TC0 and TC1
	TFLG1 = (TFLG1_C0F_MASK | TFLG1_C1F_MASK);    // Clear the flag in case anything is pending

	LCDprintf("DC Motors\nConfigured.");
}//end of configureDCM

//;**************************************************************
//;*                 dcmControl
//;*	Sets speed of motor based on speed in mm/s
//;*	Sets direction (fwd, reverse)
//;*
//;*	Inputs: motor - 1 = left, 2 = right
//;*			speed - 0-256 mm/s
//;*			direction - 1 = forward, 2 = reverse, 0 = brake
//;*
//;**************************************************************  
void dcmControl(unsigned char speed, unsigned char direction, unsigned char motor) {
	if(motor == 1) {
		targetA = speed;
	
		if(direction != dcmA_dir) {
			dcmA_dir = direction;
			switch(direction) {
			    case 0 :
			      dcmAbrk;
			      break;
			      
			    case 1 :
			      dcmAbrk;
			      dcmAfwd;
			      break;
			      
			    case 2 :
			      dcmAbrk;
			      dcmArev;
			      break;   
			  }
		}
	} //end of control for left motor

	else {
		targetB = speed;

		if(direction != dcmB_dir) {
			dcmB_dir = direction;
			switch(direction) {
			    case 0 :
			      dcmBbrk;
			      break;
			      
			    case 1 :
			      dcmBbrk;
			      dcmBfwd;
			      break;
			      
			    case 2 :
			      dcmBbrk;
			      dcmBrev;
			      break;   
		  }
		}
	} //end of control for right motor
}//end of dcmControl


//;**************************************************************
//;*                 speed_mms()
//;*  Return speed in mm/s converted from period in TCNT ticks
//;**************************************************************
unsigned char speed_mms(unsigned long period) {
  static unsigned char speed;
  speed = period_factor / (period * period_conversion);
  return(speed);
}//end of speed_cms()


//;**************************************************************
//;*                 getTargetSpeedA()
//;*  Return motor A target/set speed in mm/s
//;**************************************************************
unsigned char getTargetSpeedA(void) { 
  DisableInterrupts;
  return targetA;
  EnableInterrupts;
}//end of getTargetSpeedA()


//;**************************************************************
//;*                 getTargetSpeedB()
//;*  Return motor A target/set speed in mm/s
//;**************************************************************
unsigned char getTargetSpeedB(void) { 
  DisableInterrupts;
  return targetB;
  EnableInterrupts;
}//end of getTargetSpeedB()