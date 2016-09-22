//;******************************************************************************
//; adc.h - Header file for adc fucntions
//:
//; Name: Thomas Abdallah
//; Date: 2016-03-31
//;
//;******************************************************************************

#define analogBufSz 100u       // Number of elements for analog ring buffer

// A/D Channels
#define ATD0 0x00
#define ATD1 0x01
#define ATD2 0x02
#define ATD3 0x03
#define ATD4 0x04
#define ATD5 0x05
#define ATD6 0x06
#define ATD7 0x07

// ATD Configuration Constants
#define ATDCTL2_config 0b11000000 //Config for power up + fast clear
#define ATDCTL2_config_IE 0b11000010	// Power up, fast clear, interrupts enabled
#define ATDCTL3_config 0b00001010 //Config for 1 conversion, FIFO off, set FRZ bits to finish-then-freeze
#define ATDCTL4_config 0b10100001 //Config for 8-bit data, 4 sample clocks, prescaler of 4 (8MHz / 4 = 2MHz)

// Function prototypes
void configureADC(void);    // Configures the ADC
void configureADC_IE(void);	// Configures the ADC with interrupts enabled
unsigned char readADC(unsigned char chnl); 	// Starts an A/D conversion on chnl and returns value
unsigned char atdConsume(void);				// Returns the reading from the head of the buffer