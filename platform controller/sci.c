#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "TA_Header_W2016.h"  /* my macros and constants */
#include "sci.h"  // Macros and constants for timer3handler.
#include "lcd.h"
#include "timer.h"

// Fixed length array for SCI Rx data
static unsigned char sciBuf[sciBufSz]; 	// Buffer for SCI Rx Data
static unsigned char sciIn = 0;         // Index where next element will be written
static unsigned char sciOut = 0;		// Index where next element will be read
static unsigned char sciCount = 0;		// Number of filled elements in the buffer

//;**************************************************************
//;*                 configureSCI(void)
//;*  Configures the SCI module with parameters from sci.h
//;************************************************************** 
void configureSCI(void) {
	SCIBD = SCIBD_INIT;			// Set baud rate
	SCICR1 = SCICR1_INIT;		// // For 1 start-bit, 8 data-bits, 1 stop-bit
	SCICR2 |= (SCICR2_RE_MASK | SCICR2_TE_MASK | SCICR2_RIE_MASK);	// No interrupts, read and write enabled
	LCDprintf("SCI Configured");
}//end of configureSCI()

//;**************************************************************
//;*                 putcSCI()
//;*  Writes a single character to SCI
//;*  Input: char cx = character to write
//;************************************************************** 
void putcSCI(unsigned char cx) {
	while(!(SCISR1 & SCISR1_TDRE_MASK));	// Wait for the transmit data register to be empty
	SCIDRL = cx;	// Write the character to the transmit data register
}//end putcSCI()

//;**************************************************************
//;*                 putsSCI()
//;*  Writes a string to SCI
//;*  Input: char *str = string to write
//;************************************************************** 
void putsSCI(unsigned char *str) {
	while(*str)
		putcSCI( *str++ );
}

//;**************************************************************
//;*                 getcSCI()
//;*  Reads a single character from SCI
//;*  Polling.
//;************************************************************** 
unsigned char getcSCI(void) {
	while(!(SCISR1 & SCISR1_RDRF_MASK));
	return(SCIDRL);	
}

//;**************************************************************
//;*                 sciHandler()
//;*  	Reads data into the sciBuffer
//;*	Handles SCI Rx interrupt
//;************************************************************** 
interrupt 20 void sciHandler(void) {
	// When msgStatus = 1, we are in the middle of reading a valid message.
	// When msgStatus = 0, we throw away everything until we see a '<' which is the start
	// of a valid message.
	// We keep reading into the buffer until the buffer is full, or we read '>' which is
	// the end of a valid message
	static unsigned char msgStatus = 0; //Keeps track of beginning/end of a valid message.
	
	static unsigned char garbage;
	garbage = SCISR1; //Clear interrupt flag
		
	// if there is space in the buffer add the SCI reading
	if(sciCount < sciBufSz) {
	  if(msgStatus == 0) {
	    // Look for start of message symbol.
	    if(SCIDRL == msgStart) {
			msgStatus = 1;
	    }
	  }//end of logic for looking for the start of a message
	  
	  else {
      // Check for end of message symbol.
	    if(SCIDRL == msgEnd) {
			msgStatus = 0;
	    } 
	    else {

			if(SCIDRL == HB) {
	    	TOGGLE_LEDS;
        	putcSCI(HB);
        	setHBtimer(); // Delay the heartbeat timer again.
			}
			else {
				sciBuf[sciIn] = SCIDRL;	// Add the reading to the buffer
		    	sciCount ++;				    // Keep track of number of readings in the buffer
		    	sciIn = ((sciIn + 1) % sciBufSz);	// Change in to next available spot
			}
	    }
	  }// end of logic for looking for end of message
	}//end of buffer filling
	else {
		LCDprintf("RS232 Buffer\nFull!!!");
		garbage = SCIDRL;
	}
}//end of sciHandler


//;**************************************************************
//;*                 consumeSCI(void)
//;*  Returns the oldest value from the SCI buffer
//;*  Returns NULL if nothing read
//;**************************************************************   
unsigned char consumeSCI(void) {
  static unsigned char tmpRead;
  
  // Check if buffer is empty  
  if(sciCount == 0){
    return (bufEmpty);
  }
  
  tmpRead = sciBuf[sciOut];				// Grab value from the buffer
  //sciBuf[sciOut] = bufEmpty;			// Write null to view prod/cons in debugger
  sciOut = ((sciOut + 1) % sciBufSz);

  DisableInterrupts;  // Both prod & cons change sciCount var
  sciCount --;
  EnableInterrupts;   // End of critical region
  return (tmpRead);
}//end of consumeSCI