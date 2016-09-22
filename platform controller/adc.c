#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "TA_Header_W2016.h"  /* my macros and constants */
#include "adc.h"

// Fixed length array for ADC readings
static unsigned char analogBuf[analogBufSz]; // Buffer for adc readings
static unsigned char analogIn = 0;           // Index where next element will be written
static unsigned char analogOut = 0;          // Index where next element will be read
static unsigned char analogCount = 0;        // Number of filled elements in the buffer

//;**************************************************************
//;*                 readADC(chnl)
//;*  Reads value from ATD channel (chnl)
//;**************************************************************   
unsigned char readADC(unsigned char chnl) {
  ATDCTL5 = chnl; // Starts a conversion on ATD channel
  while(!(ATDSTAT0 & ATDSTAT0_SCF_MASK)); // Wait for A/D conversion to finish
  return(ATDDR0H);
}//end of readADC

//;**************************************************************
//;*                 configureADC(void)
//;*  Configures the ADC with parameters per adc.h
//;**************************************************************   
void configureADC(void) {
  ATDCTL2 = ATDCTL2_config; //Config for power up + fast clear
  ATDCTL3 = ATDCTL3_config; //Config for 1 conversion, FIFO off, set FRZ bits to finish-then-freeze
  ATDCTL4 = ATDCTL4_config; //Config for 8-bit data, 4 sample clocks, prescaler of 4 (8MHz / 4 = 2MHz)
}//end of configureADC

//;**************************************************************
//;*                 configureADC_IE(void)
//;*  Configures the ADC with parameters per TA_Header_W2016.h
//;*  Enables interrupts for ATD conversion sequence complete
//;**************************************************************   
void configureADC_IE(void) {
  ATDCTL2 = ATDCTL2_config_IE; //Config for power up + fast clear
  ATDCTL3 = ATDCTL3_config; //Config for 1 conversion, FIFO off, set FRZ bits to finish-then-freeze
  ATDCTL4 = ATDCTL4_config; //Config for 8-bit data, 4 sample clocks, prescaler of 4 (8MHz / 4 = 2MHz)
}//end of configureADC

//;**************************************************************
//;*                 atdHandler (atd interrupt handler)
//;*  ATD conversion is finished, 
//;**************************************************************   
interrupt 22 void atdHandler(void) {
	// if there is space in the buffer add the adc reading
	if(analogCount < analogBufSz) {
		analogBuf[analogIn] = ATDDR0H;	// Add the reading to the buffer
		analogCount ++;				// Keep track of number of readings in the buffer
		analogIn = ((analogIn + 1) % analogBufSz);	// Change in to next available spot
	}//end of buffer filling
  
  ATDSTAT0 |= (ATDSTAT0_SCF_MASK);  // Clear interrupt flag
}//end of atdHandler

//;**************************************************************
//;*                 consumeADC(void)
//;*  Returns the oldest value from the analog buffer
//;*  Returns NULL if nothing read
//;**************************************************************   
unsigned char atdConsume(void) {
  static unsigned char tmpRead;
  
  // Check if buffer is empty  
  if(analogCount == 0){
    return (0x00);
  }
  
  tmpRead = analogBuf[analogOut];
  analogOut = ((analogOut + 1) % analogBufSz);

  DisableInterrupts;  // Both prod & cons change analogCount var
  analogCount --;
  EnableInterrupts;   // End of critical region
  return (tmpRead);
}//end of configureADC