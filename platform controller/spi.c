#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "TA_Header_W2016.h"  /* my macros and constants */
#include "spi.h"

//;**************************************************************
//;*                 configureSPI()
//;*  Configures the SPI module with parameters from spi.h
//;**************************************************************   
void configureSPI(void) {
	SPIBR = SPIBR_1MHz;		// Set SPI data rate
	SPICR1 = SPICR1_INIT;
	SPISR = SPISR_INIT;
	CONFIGURE_SPI_CS;		// Configure pins for CS1 and CS2
}

//;**************************************************************
//;*                 putcSPI(char cx)
//;*  Outputs a character (cx) to the SPI port usinig polling in
//;*  half-duplex mode.
//;**************************************************************   
void putcSPI(unsigned char cx) {
	static unsigned char temp;
	while(!(SPISR & SPISR_SPTEF_MASK));	// Wait until write is permissible
	SPIDR = cx;	// Write the character to the SPI data register
	while(!(SPISR & SPISR_SPIF_MASK));	// Wait until write is complete
	temp = SPIDR;	// Clear the SPIF flag by reading the data register
}

//;**************************************************************
//;*                 configureDAC()
//;*	Configure the MAX5512 DAC for S4 project operation
//;**************************************************************   
void configureDAC(void) {
	ENABLE_CS_DAC;	// Clear the DAC CS pin.
	
	putcSPI((DAC_VREF3 >> SHIFT_NIBBLE) | (DAC_ON_CTRL));
	putcSPI((DAC_VREF3 << SHIFT_NIBBLE) | (DAC_SUB));
	
	DISABLE_CS_DAC;	// Set the DAC CS pin.
}

//;**************************************************************
//;*                 writeDAC()
//;*	Set the DAC output voltage with an 8-bit input value
//;**************************************************************   
void writeDAC(unsigned char data, unsigned char chnl) {
	ENABLE_CS_DAC;	// Clear the DAC CS pin.
	
	putcSPI((data >> SHIFT_NIBBLE) | (chnl));
	putcSPI((data << SHIFT_NIBBLE) | (DAC_SUB));

	DISABLE_CS_DAC;	// Set the DAC CS pin.
}