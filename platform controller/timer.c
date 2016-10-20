#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "TA_Header_W2016.h"  /* my macros and constants */
#include "timer.h"  // Macros and constants for timer3handler.
#include "lcd.h"
#include "servo.h"
#include "dcm.h"
#include "spi.h"

// Global variable to enable or disable the heartbeat alarm clock
unsigned char hb_alarm_state = 0;  // 0 = disabled, 1 = enabled.

// Temporary global variables to allow for tuning of gains
unsigned char gainP = 2;
unsigned char gainI = 2;

// Global variables for DC Motor Encoder processing & control
static signed long edgeA1; //Encoder A, time of 1st rising edge
static signed long edgeA2; //Encoder A, time of 2nd rising edge
static unsigned char edgeA1ovf;   //Encoder A, value of overflowCount at 1st rising edge
static unsigned char edgeA2ovf;   //Encoder A, value of overflowCount at 2nd rising edge
static unsigned char periodAstart = 0;  //ISR logic to track 1st or 2nd rising edge.
static unsigned long periodA;     //Encoder A, measured period in TCNT ticks
static unsigned char speedA;       //Speed of motor A in mm/s converted from periodA

static signed long edgeB1; //Encoder B, time of 1st rising edge
static signed long edgeB2; //Encoder B, time of 2nd rising edge
static unsigned char edgeB1ovf;   //Encoder B, value of overflowCount at 1st rising edge
static unsigned char edgeB2ovf;   //Encoder B, value of overflowCount at 2nd rising edge
static unsigned char periodBstart = 0;  //ISR logic to track 1st or 2nd rising edge.
static unsigned long periodB;     //Encoder B, measured period in TCNT ticks
static unsigned char speedB;       //Speed of motor B in mm/s converted from periodB

static unsigned char overflowCount = 0; //To handle timer wrapping for IC functionality

// Fixed length arrays for period readings
unsigned char periodAbuf[periodBufSz];  // Buffer for adc readings
unsigned char periodAin = 0;            // Index where next element will be written
unsigned char periodAout = 0;           // Index where next element will be read
unsigned char periodAcount = 0;          // Number of filled elements in the buffer
unsigned char periodBbuf[periodBufSz];  // Buffer for adc readings
unsigned char periodBin = 0;            // Index where next element will be written
unsigned char periodBout = 0;           // Index where next element will be read
unsigned char periodBcount = 0;          // Number of filled elements in the buffer

// Heartbeat loop counter, if this gets to 40 we haven't received a heartbeat in 1.2 seconds
static unsigned char hbCount = 0;

// Error between set speed and recorded speed.
static signed char errorA;   // Error between speedA (mm/s from encoder) to targetA which is currently set motor speed in mm/s
static signed char errorB;   // Error between speedB (mm/s from encoder) to targetB which is currently set motor speed in mm/s
static signed long errorA_I = 0;  // Error for the integral controller
static signed long errorB_I = 0;  // Error for the integral controller
static unsigned char PWMDTY_A = 0;      // Duty cycle for Motor A (from 0 - 255)
static unsigned char PWMDTY_B = 0;      // Duty cycle for Motor B (from 0 - 100)
static signed int PWMDTY_A_calc = 0;   // Calculated new duty cycle from PI controller
static signed int PWMDTY_B_calc = 0;   // Calculated new duty cycle from PI controller


//;**************************************************************
//;*                 configureTimer(void)
//;*  Configures the timer module with parameters for PWM operation
//;**************************************************************   
void configureTimer(void) {
  TSCR1 = TSCR1_INIT; // Turn on timer module and enable fast-clear and freeze in debug
  TSCR2 = TSCR2_INIT; // Set pre-scaler to 4 for finest resolution @50Hz PWM frequency
}//end of configureTimer


//;**************************************************************
//;*                 msDelay(time)
//;*  Delay program execution by time mS (busy wait)
//;*  Delays on TC7
//;**************************************************************
void msDelay(unsigned char time) {
  // 1 TCNT tick = 0.5uS so 2000 TCNT ticks = 1mS
  volatile unsigned char count;

  SET_OC_ACTION(7,OC_OFF);     // Set TC7 to not touch the port pin
  TC7 = TCNT + TCNT_mS; // Set first OC event timer (for 1mS)
  TIOS |= TIOS_IOS7_MASK; // Enable TC1 as OC

  for(count = 0; count < time; count ++)
  {
    while(!(TFLG1 & TFLG1_C7F_MASK)); // Wait for the OC event
    TC7 += TCNT_mS;
  }

  TIOS &= LOW(~TIOS_IOS7_MASK);  // Turn off OC on TC1

}//end of msDelay


//;**************************************************************
//;*                 usDelay(time)
//;*  Delay program execution by time uS (busy wait)
//;*  Delays on TC7
//;**************************************************************
void usDelay(unsigned char time) {
  // 1 TCNT tick = 0.5uS so 2 TCNT ticks = 1uS
  volatile unsigned char count;

  SET_OC_ACTION(7,OC_OFF);     // Set TC7 to not touch the port pin
  TC7 = TCNT + TCNT_uS; // Set first OC event timer (for 1mS)
  TIOS |= TIOS_IOS7_MASK; // Enable TC7 as OC

  for(count = 0; count < time; count ++)
  {
    while(!(TFLG1 & TFLG1_C7F_MASK)); // Wait for the OC event
    TC7 += TCNT_uS;
  }

  TIOS &= LOW(~TIOS_IOS7_MASK);  // Turn off OC on TC7

}//end of usDelay


//;**************************************************************
//;*                 getPeriodA()
//;*  Return value of Period A
//;**************************************************************
unsigned int getPeriodA(void) {
  static unsigned int period;
  DisableInterrupts;  //Protect from ISR changing global variable
  period = (unsigned int)periodA;
  EnableInterrupts;
  return(period);

  /*
  static unsigned int period;
  
  // Check if buffer is empty  
  if(periodAcount == 0){
    return (0x0000);
  }
  
  period = periodAbuf[periodAout];
  periodAout = ((periodAout + 1) % periodBufSz);

  DisableInterrupts;  // Both prod & cons change periodAcount var
  periodAcount --;
  EnableInterrupts;   // End of critical region
  return (period);
  */

}//end of getPeriodA


//;**************************************************************
//;*                 getPeriodB()
//;*  Return value of Period B
//;**************************************************************
unsigned int getPeriodB(void) {
  static unsigned int period;
  DisableInterrupts;  //Protect from ISR changing global variable
  period = (unsigned int)periodB;
  EnableInterrupts;
  return(period);
}//end of getPeriodB


//;**************************************************************
//;*                 timer0Handler()
//;*  Handles IC function for Encoder A (Left DC Motor)
//;**************************************************************
interrupt 8 void timer0Handler(void) {
  if(periodAstart == 0) {
    edgeA1 = ENCA_Timer;
    edgeA1ovf = overflowCount;   
    periodAstart = 1;
  }//end of logic for handling the first rising edge
  else {
    edgeA2 = ENCA_Timer;
    edgeA2ovf = overflowCount;
    periodA = (edgeA2 - edgeA1) + (OVF_Factor * (edgeA2ovf - edgeA1ovf));
    speedA = speed_mms(periodA);
    periodAstart = 0;
  }//end of logic for handling the second rising edge
}//end of timer0Handler()


//;**************************************************************
//;*                 timer1Handler()
//;*  Handles IC function for Encoder B (Right DC Motor)
//;**************************************************************
interrupt 9 void timer1Handler(void) {
  if(periodBstart == 0) {
    edgeB1 = ENCB_Timer;
    edgeB1ovf = overflowCount;   
    periodBstart = 1;
  }//end of logic for handling the first rising edge
  else {
    edgeB2 = ENCB_Timer;
    edgeB2ovf = overflowCount;
    periodB = (edgeB2 - edgeB1) + (OVF_Factor * (edgeB2ovf - edgeB1ovf));
    speedB = speed_mms(periodB);
    periodBstart = 0;
  }//end of logic for handling the second rising edge
}//end of timer1Handler()


//;**************************************************************
//;*                 timerOverflowHandler()
//;*  Increments global variable to track timer overflow events
//;**************************************************************
interrupt 16 void timerOverflowHandler(void) {
  overflowCount ++; //Increment the overflow counter
  (void)TCNT;   //To clear the interrupt flag with fast-clear enabled.
}//end of timerOverflowHandler()


//;**************************************************************
//;*                 configureHB()
//;*  Set up Timer Channel 5 to act as a heartbeat alarm clock
//;**************************************************************
void configureHB(void) {
  TIOS |= TIOS_IOS5_MASK;       // Enable TC5 as OC for heartbeat alarm
  SET_OC_ACTION(5,OC_OFF);      // Set TC5 to not touch the port pin
  TC5 = TCNT + TCNT_30mS;       // Delay 30mS
  TIE |= TIOS_IOS5_MASK;        // Enable interrupts on timer channel 5
}


//;**************************************************************
//;*                 timer5Handler()
//;*  Heartbeat alarm clock on timer channel 5
//;**************************************************************
interrupt 13 void timer5Handler(void) {
  // If heartbeat alarm clock is disabled, just pretend we got one MmmmmmKayyyyy
  if(hb_alarm_state == 0) {
    hbCount = 0;
  }
  
  // If less than 1.8 seconds, all okay.
  if(hbCount < hbLimit) {
    TC5 += TCNT_30mS; // Delay 30mS  
    hbCount ++;
  }
  else {
    // Heartbeat is dead :(
    // Shut er down Fred.    
    LCDprintf("Lost heartbeat.\n");
    
    dcmAbrk;  //Shut off the left DC motor
    dcmBbrk; //Shut oft the right DC motor
        
    FORCE_OC_ACTION_NOW(SERVO1,OC_GO_LO); // Force pin off    
    SET_OC_ACTION(SERVO1,OC_OFF);         // Set TC0 to not toggle the port pin.
    
    //DISABLE_5VA;                //Shut off the secondary power supply    
    
    LCDprintf("He's dead Jim!\nLost heartbeat.");
    
    DisableInterrupts;
    
    for(;;);  // Infinite loop of sadness and failure.
  }
}//end of timer5Handler()


//;**************************************************************
//;*                 setHBtimer()
//;*  We got a new heartbeat, woohoo, set the hbCount back to zero.
//;**************************************************************
void setHBtimer(void) {
  DisableInterrupts;
  hbCount = 0;
  EnableInterrupts;
  
  if(hb_alarm_state == 0) {
    set_hb_alarm_state(1);
  }    
  
}


//;**************************************************************
//;*                 set_hb_alarm_state(unsigned char state)
//;*  Set the state of the hb alarm clock
//;*  state 0 = alarm clock off
//;*  state 1 = alarm clock on
//;**************************************************************
void set_hb_alarm_state(unsigned char state) {
  DisableInterrupts;
  hb_alarm_state = state;
  EnableInterrupts;
}


//;**************************************************************
//;*                 configureMotorControl()
//;*  Set up Timer Channel 6 to act as a clock that will cause
//;*  Motor Control loop to run every 30mS
//;**************************************************************
void configureMotorControl(void) {
  TIOS |= TIOS_IOS6_MASK;       // Enable TC6 as OC for motor control loop
  SET_OC_ACTION(6,OC_OFF);      // Set TC6 to not touch the port pin
  TC6 = TCNT + TCNT_30mS;       // Delay 30mS
  TIE |= TIOS_IOS6_MASK;        // Enable interrupts on timer channel 5
}


//;**************************************************************
//;*                 timer6Handler()
//;*  Motor control timer on channel 6
//;**************************************************************
interrupt 14 void timer6Handler(void) {
  DisableInterrupts;

  // Calculate error for each motor.
  // Positive error = Too fast
  // Negative error = Too slow
  //errorA = speedA - getTargetSpeedA();
  //errorB = speedB - getTargetSpeedB();
  errorA = getTargetSpeedA() - speedA;
  errorB = getTargetSpeedB() - speedB;

  // I term update if we aren't on a rail
  if ((PWMDTY_A > MIN_DRIVE_PWM) && (PWMDTY_A < MAX_DRIVE_PWM)) {
    errorA_I += errorA;
  }

  if ((PWMDTY_B > MIN_DRIVE_PWM) && (PWMDTY_B < MAX_DRIVE_PWM)) {
    errorB_I += errorB;
  }

  // P control calculation
  PWMDTY_A_calc = (gainP * errorA) + (gainI * errorA_I);
  PWMDTY_B_calc = (gainP * errorB) + (gainI * errorB_I);

  // Clip the output to 0-255 (max PWM drive values)
  if (PWMDTY_A_calc > MAX_DRIVE_PWM) {
    PWMDTY_A = MAX_DRIVE_PWM;
  }
  else if (PWMDTY_A_calc < MIN_DRIVE_PWM) {
    PWMDTY_A = MIN_DRIVE_PWM;
  }
  else {
    PWMDTY_A = (unsigned char)PWMDTY_A_calc;
  }

  if (PWMDTY_B_calc > MAX_DRIVE_PWM) {
    PWMDTY_B = MAX_DRIVE_PWM;
  }
  else if (PWMDTY_B_calc < MIN_DRIVE_PWM) {
    PWMDTY_B = MIN_DRIVE_PWM;
  }
  else {
    PWMDTY_B = (unsigned char)PWMDTY_B_calc;
  }

  // Set the motor speeds
  dcmPWM_SET_DUTY_A(PWMDTY_A);  // Set motor A PWM duty cycle
  dcmPWM_SET_DUTY_B(PWMDTY_B);  // Set motor B PWM duty cycle

  TC6 = TCNT + TCNT_30mS;       // Delay 30mS

  EnableInterrupts;
}//end of timer6Handler()