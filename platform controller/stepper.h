//;******************************************************************************
//; stepper.h - Header file for stepper motor fucntions
//:
//; Name: Thomas Abdallah
//; Date: 2016-03-31
//;
//;******************************************************************************
#include "TA_Header_W2016.h"  /* my macros and constants */

// Stepper Port Patterns & Masks
#define StepperPort PTT       //Driver stepper motor on port T
#define StepperDDR DDRT       //Driver stepper motor on port T
#define StepperPortMask 0b00001111    //AND StepperPort with this to clear the upper nibble
#define StepperPortConfig 0b11110000  //Configure upper nibble as outputs

// Limit switch Port Patterns & Masks
#define Switch_R_Mask 0b10000000	//Port PAD07
#define Switch_L_Mask 0b01000000	//Port PAD06
#define SwitchPort PTAD				//Switches on ATD port

// Macros for enabling and reading switch data
#define Switch_Init SET_BITS(ATDDIEN, (Switch_R_Mask | Switch_L_Mask))
#define Switch_Check (((SwitchPort) & (Switch_L_Mask | Switch_R_Mask)) != 0) //Logical False until a switch is pressed


// Patterns for driving the stepper motor (PT7 - PT4)
#define p0 0b10000000
#define p1 0b10100000
#define p2 0b00100000
#define p3 0b01100000
#define p4 0b01000000
#define p5 0b01010000
#define p6 0b00010000
#define p7 0b10010000

// Values for the RTICTL register (12 x 2^16) for ~50mS interrupts
#define RTICTL_INIT 0x7B

// Value to set delay between LED flashing (10*50mS = 500mS, 20*50mS = 1sec)
#define CountLimit 5

// 3-bit upcounter mask
// Used in stepper motor logic to step through patterns 0-7
#define countMask 0b00000111

// Stepper speed constants
#define stepperSpeed_default 10000u	// For 10mS per step
#define stepperSpeed_slow 65535u	// Slowest possible stepper motion without wrapping
#define stepperSpeed_fast 5000u 	// Fast speed... 5mS per step baby
#define speedFactor 2 				// Number of TCNT ticks per uS
#define CCW_HALF -1
#define CCW_FULL -2
#define NO_STEP 0
#define CW_HALF 1
#define CW_FULL 2
#define minSteps 20 				// Minimum number of steps to move before checking limit switches
									// Forces stepper to move away from the limit switch before checking for a collision
#define CONTINUOUS_STEPPING 0xFFFFu	// To tell the moveStepper() function to move continuously (until a limit switch is pressed)
#define deg2stepsFactor 100u 		// More precision on the conversion

// Function prototypes
void configureStepper(void);	// Configure the upper nibble of Port T to drive the stepper
void setStepperSpeed(unsigned int speed);	// Function to change global var stepTime (includes mS to TCNT tick conv)
void homeStepper(void);			// Home the stepper motor & save global variable stepRange
void moveStepper(unsigned int speed, signed char direction, unsigned int numSteps);	// Move the stepper based on input parameters
unsigned int getStepperPosition(void);	// Returns position of stepper in number of steps from limit switch
unsigned char getStepperState(void);	// Returns state of position (-2, -1, 0, 1, 2)
void setStepperPos(unsigned char position);