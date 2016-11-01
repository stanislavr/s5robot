//;******************************************************************************
//; timer.h - Header file for operating the RC servo on timer 3 (PT3)
//:
//; Name: Thomas Abdallah
//; Date: 2016-03-31
//;
//;******************************************************************************

// Action values for OC function
#define OC_OFF		0x00
#define OC_TOGGLE	0x01
#define OC_GO_LO	0x02
#define OC_GO_HI	0x03

// To identify a rising edge or falling edge for servo operation
#define servoRiseEdge 1
#define servoFallEdge 0

// Timer configuration parameters
#define TSCR1_INIT 0b10110000	// Turn on timer module and enable fast-clear and freeze in debug
//#define TSCR2_INIT 0b00000010	// Set pre-scaler to 4 for finest resolution @50Hz PWM frequency
#define TSCR2_INIT 0b10000010	// Set pre-scaler to 4 for finest resolution @50Hz PWM frequency, overflow interrupt enabled
#define TCTL4_INIT 0b00000101	// Capture on rising edges of TC0 and TC1

// Define TCTL_1_2 as a single 16 bit register made up of TCTL1 and TCTL2 together
#define TCTL_1_2 (*(volatile word * const) &TCTL1)

#define TCNT_30mS 60000u			// Number of TCNT ticks for 30mS with prescaler of 4
#define TCNT_mS 2000			// Number of TCNT ticks for 1mS with prescaler of 4
#define TCNT_uS 2 				// Number of TCNT ticks for 1uS with prescaler of 4

// DC Motor Encoder Constants & Macros
#define ENCA_Timer TC0 		//Timer channel for Encoder A readings
#define ENCB_Timer TC1 		//Timer channel for Encoder B readings
#define OVF_Factor 65536l	//2^16
#define periodBufSz 50u     // Number of elements for period ring buffer

// Heartbeat constants
#define hbLimit 100  // For a 1.8s delay, 1800mS / 30mS = 60, so 60 interrupts before alarm triggered for real 
#define hbOff 0        // Heartbeat alarm turned off
#define hbOn 1        // Heartbeat alarm turned on

// Function prototypes
void configureTimer(void);			// Configure the timer module at startup
void msDelay(unsigned char time);	// Delay by time ms using OC polling
void usDelay(unsigned char time);	// Delay by time us using OC polling
unsigned int getPeriodA(void);		// Accessor function for periodA var
unsigned int getPeriodB(void);		// Accessor function for periodB var
void setHBtimer(void);				// Restore hbCount to zero for another 1.2s alarm delay
void configureHB(void);				// Configure timer 5 for heartbeat alarm
void set_hb_alarm_state(unsigned char state);  // Enable or disable the hearbeat alarm clock.