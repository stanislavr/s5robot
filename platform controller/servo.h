//;******************************************************************************
//; servo.h - Header file for operating the RC servo on timer 2 (PT2)
//:
//; Name: Thomas Abdallah
//; Date: 2016-04-05
//;
//;******************************************************************************

#define SERVO1 2 // Port number for servo 1 (PT2) (on camera connector)
#define SERVO2 3 // Pattern number servo 2 (PT3) (on aux connector)

// Set the servo up for a 20mS period = 40000 timer ticks
#define servoPeriod 40000u

// Set the servo position to centre (1500uS = 3000 timer ticks)
#define servoCentre 3000

// Conversion factor: A/D reading to timer ticks (for Servo operation from ATD)
#define ad2ticksFactor 14

// Conversion factor: degrees to timer ticks
#define degTicksFactor 20

// Offset: # of timer ticks for 0 degree rotation of servo (600uS = 1200 ticks)
#define ticksOffset 1200

// Limits for servo in degrees
#define servoLimit_Upper 180
#define servoLimit_Lower 70
#define servoLimit_Home 120

// Function prototeyps
void configureServo(void);							// Configure servo to operate on OC2
void setServoADC(unsigned char ADCreading);			// Used to access the static variable servo2onTime from main
void setServoPosition(unsigned char degrees);		// Sets servo position in degrees