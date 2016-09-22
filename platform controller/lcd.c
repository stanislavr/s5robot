#include <hidef.h>      /* common defines and macros */
//#include <stdarg.h>
#include <stdio.h>
#include "derivative.h"      /* derivative-specific definitions */
#include "TA_Header_W2016.h"  /* my macros and constants */
#include "lcd.h"
#include "timer.h"

//;**************************************************************
//;*                 configureLCD()
//;*  Configures the LCD module with parameters from lcd.h
//;**************************************************************   
void configureLCD(void) {
	// Configure LCD Port as outputs
	LCD_PORT_DDR = 0xFF;	// FF for all pins as outputs

	// Below code copied directly from Bills slides.
	// run through sync sequence from datasheet to start 4-bit interface    
    LCD_E_HI;
    LCD_BUS( 0x03 );      // wake up display & sync
    LCD_E_LO;
    
    msDelay( 5 );

    LCD_E_HI;
    LCD_BUS( 0x03 );      // wake up display & sync
    LCD_E_LO;

    msDelay( 1 );   
    
    LCD_E_HI;
    LCD_BUS( 0x03 );      // wake up display & sync
    LCD_E_LO;
    
    LCD_E_HI;
    LCD_BUS( 0x02 );      // wake up display & sync - go to 4-bit mode
    LCD_E_LO;

	msDelay( 2 );

// now that we're sync'd and in 4-bit mode, issue commands to configure the display
    LCDcmd( LCD_CMD_FUNCTION | LCD_FUNCTION_4BIT | LCD_FUNCTION_2LINES | LCD_FUNCTION_5X8FONT );
    LCDcmd( LCD_CMD_DISPLAY | LCD_DISPLAY_OFF );
    LCDcmd( LCD_CMD_CLEAR );
    LCDcmd( LCD_CMD_ENTRY | LCD_ENTRY_MOVE_CURSOR | LCD_ENTRY_INC );
    LCDcmd( LCD_CMD_DISPLAY | LCD_DISPLAY_ON | LCD_DISPLAY_NOCURSOR | LCD_DISPLAY_NOBLINK );
}


//;**************************************************************
//;*                 LCDdata(cx)
//;*  Writes 8 bits of data to the LCD
//;**************************************************************   
void LCDdata(unsigned char cx) {
	LCD_E_LO;
	SET_BITS(LCD_PORT, LCD_RS_BIT);		// Set RS high for data
	CLEAR_BITS(LCD_PORT, LCD_RW_BIT);	// Write mode
	LCD_E_HI;
	LCD_BUS(HI_NYBBLE(cx));
	LCD_E_LO;
	LCD_E_HI;
	LCD_BUS(LO_NYBBLE(cx));
	LCD_E_LO;
	msDelay(2);
}

//;**************************************************************
//;*                 LCDcmd(cx)
//;*  Writes an 8-bit command to the LCD
//;**************************************************************   
void LCDcmd(unsigned char cx) {
	LCD_E_LO;
	CLEAR_BITS(LCD_PORT, LCD_RS_BIT);	// Clear RS for command
	CLEAR_BITS(LCD_PORT, LCD_RW_BIT);	// Write mode
	LCD_E_HI;
	LCD_BUS(HI_NYBBLE(cx));
	LCD_E_LO;
	LCD_E_HI;
	LCD_BUS(LO_NYBBLE(cx));
	LCD_E_LO;
	msDelay(2);
}

//;**************************************************************
//;*                 LCDputs()
//;*  Writes a string to the LCD
//;*  Input: char *str = string to write
//;************************************************************** 
void LCDputs(unsigned char *str) {
	LCDcmd(LCD_CMD_CLEAR);
	msDelay(2);
	while(*str)
		if(*str == '\n') {	// Handle linefeed characters.
			LCDcmd(LCD_CMD_SET_DDRAM | LCD_SET_LINE2);
			str++;
		}
		else {
			LCDdata( *str++ );
		}
}

//;**************************************************************
//;*                 LCDprintf()
//;*  Writes a formatted string to the LCD
//;*  Input: char *format = formatted string to write
//;*	Other input parameters for each formatted item as with printf
//;************************************************************** 
void LCDprintf(char* format, ...) {
	va_list myArgs;
	char buffer[LCD_MAX_BUF_SIZE];
	va_start(myArgs, format);
	(void)vsprintf(buffer, format, myArgs);
	va_end(myArgs);
	LCDputs((unsigned char*)buffer);
}