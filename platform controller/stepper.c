#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "TA_Header_W2016.h"  /* my macros and constants */
#include "stepper.h"
#include "timer.h"
#include "lcd.h"
#include "sci.h"

// Global variables to control the stepper
static unsigned char tableIndex; //tableIndex keeps track of last pattern in the pTable to be used
static unsigned char pTable[] = {p0, p1, p2, p3, p4, p5, p6, p7};  //lookup table for stepper values
static unsigned int stepTime; 		//Delay in TCNT ticks for stepper (to control speed)
static unsigned int stepRange;	  	//Number of steps to go from limit switch to limit switch (-2 so we try to avoid hitting the limit switches)
static unsigned int deg2steps;		//Number of steps per degree from 0 - 180
static unsigned int steps2move = CONTINUOUS_STEPPING;  //Tells ISR how many steps to take before stopping, for moveStepper() function
static unsigned int stepsTaken;	//For moveStepper() and homing function, counts the number of steps taken vs. steps2move
static unsigned int stepperPos;	//Count number of steps taken from left limit switch (this is the position from 0 - stepRange)

// -2 = CCW full-step
// -1 = CCW half-step
// 0 = off (no step)
// 1 = CW half-step
// 2 = CW full-step
static signed char stepMode; //stepMode identifies step type and direction for stepper motor

//;**************************************************************
//;*                 timer4handler
//;*  Interrupt handler for timer channel 4.
//;*  Timer channel 4 used to control stepper motor
//;*  Write new stepper pattern, incremented based on stepMode
//;**************************************************************   
interrupt 12 void timer4handler(void) {
    tableIndex = (countMask & (tableIndex + stepMode)); //Increment to the next spot in the pattern lookup table
  	StepperPort = (StepperPortMask & StepperPort) | pTable[tableIndex]; //Take a step        
    TC4 += stepTime; 	// Set up timer compare channel for delay to next step

    if(stepMode != NO_STEP) {
  	  stepperPos = stepperPos + stepMode;		  // Increment the step counter if the motor is moving
  		stepsTaken ++;
  	}
    
    // Stop stepping if a limit switch is pressed	  
    // Stop stepping if stepCount > steps2move
	  if((Switch_Check  && (stepsTaken > minSteps)) || (stepsTaken > steps2move)) {
	    stepMode = NO_STEP;
	    steps2move = CONTINUOUS_STEPPING;
	  } 
}//end of timer4handler

//;**************************************************************
//;*                 setStepperSpeed(speed)
//;*  Sets stepTime in TCNT ticks
//;*  Input: Speed in uS
//;**************************************************************   
void setStepperSpeed(unsigned int speed) {
  DisableInterrupts;  // Prevent the OC interrupt from reading
  stepTime = (speed/speedFactor);   // Convert from time in uS to a delay in TCNT ticks
  EnableInterrupts;
}//end of setStepperSpeed

//;**************************************************************
//;*                 homeStepper(void)
//;*  Drive stepper until one limit switch pressed, reverse and count steps in global var
//;**************************************************************   
void homeStepper(void) {
	stepRange = 0;		//Reset the stepper range count
	//stepCount = 0;		//Reset the step counter
	
	// Find the left limit switch
	stepsTaken = 0;
	stepMode = CCW_HALF;

	while(stepMode);	//Wait until a limit switch is pressed
	stepMode = NO_STEP;
	    
	// Find the right limit switch
	stepperPos = 0;		    // Reset the position counter
	stepsTaken = 0;
	stepMode = CW_HALF;
	while(stepMode);		//Wait until a limit switch is pressed
	stepMode = NO_STEP;
	stepRange = stepperPos;	//Full step range has been achieved

	// Go back to centre position
	stepsTaken = 0;
	stepMode = CCW_HALF;
	while(stepsTaken < (stepRange/2));	// stepRange/2 = centre position
	stepMode = NO_STEP;

	// Make step range 2 full steps less than full range so we don't hit limit switches
	stepRange = stepRange - 4;

	// Calculate degrees to steps conversion factor
	deg2steps = (stepRange * deg2stepsFactor) / 180;

	// Stepper should now be homed, (stepRange/2) should safely take us +/- 90 degrees	
}//end of homeStepper

//;**************************************************************
//;*                 configureStepper(void)
//;*  Configures the upper nibble of Port T as outputs for
//;*  driving the stepper.
//;**************************************************************   
void configureStepper(void) {
	DisableInterrupts;
	
	SET_BITS(StepperDDR, StepperPortConfig);            //Configure upper nibble of port T as outputs
	StepperPort = (StepperPortMask & StepperPort) | p0; //Write initial value to port T
	Switch_Init;                                        //Set the proper pins as inputs for the limit switches
  
	stepMode = NO_STEP;   	//Make initial stepper mode CW half-steps.
	tableIndex = 0; 	      //Initialize table index to 0 (start of pTable)  
	setStepperSpeed(stepperSpeed_default);

  TC4 = TCNT + stepTime;            // Preset TC4 for first OC event
  TIOS |= TIOS_IOS4_MASK;           // Enable TC4 as OC
  SET_OC_ACTION(4,OC_OFF);		  // Set TC4 to not touch the fucking port pin you idiot
  TIE |= TIOS_IOS4_MASK;            // Enable interrupts for TC4

  LCDprintf("GO HOME STEPPER");
  EnableInterrupts;
  homeStepper();  // Home the stepper motor to 0 degrees
  LCDprintf("I AM HOME\n-Love Stepper");	
}//end of configureStepper

//;**************************************************************
//;*                moveStepper()
//;*  Inputs:
//;*  speed -in uS per step
//;*  direction -stepMode input as defined in stepper.h
//;*  numSteps -number of steps to take, or step continuously
//;**************************************************************   
void moveStepper(unsigned int speed, signed char direction, unsigned int numSteps) { 
  stepsTaken = 0;
  setStepperSpeed(speed);
  steps2move = numSteps;
  
  /*
  if((numSteps + stepperPos) > stepRange) {
  	steps2move = stepRange - stepperPos;
  }
  else {
  	steps2move = numSteps;  //Set the limit for number of steps
  }
  */
  
  stepMode = direction; //Start moving the motor
}//end of movestepper

//;**************************************************************
//;*                getStepperPosition()
//;*  Returns position in steps from left switch
//;**************************************************************   
unsigned int getStepperPosition(void) {
	static unsigned int position;
	DisableInterrupts;
	position = stepperPos;
	EnableInterrupts;
	return (position);
}//end of get stepper position

//;**************************************************************
//;*                getStepperState()
//;*  Returns state of stepper (-2 to +2)
//;**************************************************************   
unsigned char getStepperState(void) {
	static unsigned char state;
	DisableInterrupts;
	state = stepMode;
	EnableInterrupts;
	return (state);
}//end of get stepper state

//;**************************************************************
//;*                setStepperPos()
//;*  Calculates number of steps needed to move stepper to a
//;*  given positon (in degrees), moves stepper to that spot
//;**************************************************************   
void setStepperPos(unsigned char position) {
	static unsigned int startPos;
	static unsigned int endPos;

  LCDprintf("Panning to: \n%i degrees", position);
	
	// Get starting position in steps from left limit
	startPos = getStepperPosition();

	// Convert from degrees to steps from left limit
	endPos = position * deg2steps / deg2stepsFactor;

	// If current position is greater than desired position, step left.
	if(startPos > endPos) {
		moveStepper(stepperSpeed_default, CCW_HALF, (startPos - endPos));
	}
	else {
		moveStepper(stepperSpeed_default, CW_HALF, (endPos - startPos));
	}		
}