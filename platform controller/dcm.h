//;******************************************************************************
//; dcm.h - Header file for DC motor functions
//:
//; Name: Thomas Abdallah
//; Date: 2016-04-06
//;
//;******************************************************************************
#include "TA_Header_W2016.h"  /* my macros and constants */
#include "timer.h"

// Note: dcm = DC Motor
// Note: Motor A = Left Side of Robot
// Note: Motor B = Right Side of Robot

#define dcmLeft 1
#define dcmRight 2

// DC Motors directions Port, DDR, Masks
#define dcmPort PORTB
#define dcmDDR DDRB
#define dcmAdir1 0b00000001
#define dcmAdir2 0b00000010
#define dcmBdir1 0b00000100
#define dcmBdir2 0b00001000

// DC Motors direction control masks
#define dcmAfwd (FORCE_BITS(dcmPort, (dcmAdir1 | dcmAdir2), dcmAdir1))
#define dcmArev (FORCE_BITS(dcmPort, (dcmAdir1 | dcmAdir2), dcmAdir2))
#define dcmAbrk CLEAR_BITS(dcmPort, (dcmAdir1 | dcmAdir2))
#define dcmBfwd (FORCE_BITS(dcmPort, (dcmBdir1 | dcmBdir2), dcmBdir2))
#define dcmBrev (FORCE_BITS(dcmPort, (dcmBdir1 | dcmBdir2), dcmBdir1))
#define dcmBbrk CLEAR_BITS(dcmPort, (dcmBdir1 | dcmBdir2))

// DC Motor PWM signal control
#define MODE_8BIT 0x00		//8-bit mode for all PWM channels
#define NO_PRESCALE 0x00	//Don't prescale, count E-clocks directly for A and B
#define HALF_PRESCALE 0x01	//PWM Clock = 1/2 E-Clock
#define CLKSA_SCALE 0x01	//Makes clock SA = 1/2 Clock A
#define PWMPER 100u			//PWM period = 100 for easy control of PWM duty from 0-100%
// PWMDTY will be value between 0 and 100 (decimal)

// DC Motor PWM Macros
#define dcmPWM_CENTRE_ALIGNED SET_BITS(PWMCAE, (PWMCAE_CAE4_MASK | PWMCAE_CAE5_MASK))	// Use centre aligned for DC motor PWM signal
#define dcmPWM_LEFT_ALIGNED CLEAR_BITS(PWMCAE, (PWMCAE_CAE4_MASK | PWMCAE_CAE5_MASK))	// Use left aligned for DC motor PWM signal
#define dcmPWM_CLK_A CLEAR_BITS(PWMCLK, (PWMCLK_PCLK4_MASK | PWMCLK_PCLK5_MASK))		// Use Clock A as source for DC motor PWM signals
#define dcmPWM_CLK_SA SET_BITS(PWMCLK, (PWMCLK_PCLK4_MASK | PWMCLK_PCLK5_MASK))			// Use Clock SA as source for DC motor PWM signals
#define dcmPWM_ACTIVE_LOW CLEAR_BITS(PWMPOL, (PWMPOL_PPOL4_MASK | PWMPOL_PPOL5_MASK))	// Active low output for DC motor PWM signals
#define dcmPWM_ACTIVE_HIGH SET_BITS(PWMPOL, (PWMPOL_PPOL4_MASK | PWMPOL_PPOL5_MASK))	// Active high  output for DC motor PWM signals
#define dcmPWM_SET_PERIOD_A PWMPER4 = PWMPER					// Set period = PWMPER for DC motor PWM signals
#define dcmPWM_SET_PERIOD_B PWMPER5 = PWMPER 					// Set period = PWMPER for DC motor PWM signals
#define dcmPWM_SET_DUTY_A(dty) PWMDTY4 = dty 					// Set duty cycle for Motor A
#define dcmPWM_SET_DUTY_B(dty) PWMDTY5 = dty 					// Set duyt cycle for Motor B
#define dcmPWM_CLR_CNT_A PWMCNT4 = 0							// Reset counter for Motor A PWM
#define dcmPWM_CLR_CNT_B PWMCNT5 = 0 							// Reset counter for Motor B PWM
#define dcmPWM_ENABLE SET_BITS(PWME, (PWME_PWME4_MASK | PWME_PWME5_MASK))		// Enable both PWM channels simultaneously
#define dcmPWM_DISABLE CLEAR_BITS(PWME, (PWME_PWME4_MASK | PWME_PWME5_MASK))	// Disable both PWM channels simultaneously

// Encoder Port, DDR, Masks
#define encPort PTT
#define encDDR DDRT
#define encAmask 0b00000001
#define encBmask 0b00000010

// Speed conversion between TCNT ticks and mm/s
// speed(mm/s) = 1e7 / (TCNT ticks * 73)
#define period_factor 10000000u
#define period_conversion 73u

// Function prototypes
void configureDCM(void);	// Configure parameters for DC motor operation
void dcmControl(unsigned char speed, unsigned char direction, unsigned char motor);	// Function for open-loop control of the motors
unsigned int speed_mms(unsigned long period);	// Convert period in TCNT ticks to mm/s
unsigned char getTargetSpeedA(void);			// Returns motor A target speed in mm/s
unsigned char getTargetSpeedB(void);			// Returns motor B target speed in mm/s