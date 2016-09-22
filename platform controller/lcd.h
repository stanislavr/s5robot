//;******************************************************************************
//; lcd.h - Header file for HCS12 LCD interface
//:
//; Name: Thomas Abdallah
//; Date: 2016-04-04
//;
//;******************************************************************************
#include "TA_Header_W2016.h"  /* my macros and constants */

// LCD Port/Bus Mapping
#define LCD_PORT PORTA
#define LCD_PORT_DDR DDRA
#define LCD_E_BIT PORTA_BIT4_MASK
#define LCD_RS_BIT PORTA_BIT5_MASK
#define LCD_RW_BIT PORTA_BIT6_MASK
#define LCD_BUS_BITS 0x0F

// LCD Macros
#define LCD_E_LO CLEAR_BITS(LCD_PORT, LCD_E_BIT)
#define LCD_E_HI SET_BITS(LCD_PORT, LCD_E_BIT)
#define LCD_BUS(value) FORCE_BITS(LCD_PORT, LCD_BUS_BITS, (value))

// LCD Configuarion parameters (sent as commands)
#define LCD_CMD_FUNCTION 0b00100000
#define LCD_FUNCTION_4BIT 0x00
#define LCD_FUNCTION_2LINES 0b00001000
#define LCD_FUNCTION_5X8FONT 0x00

#define LCD_CMD_DISPLAY 0b00001000
#define LCD_DISPLAY_OFF 0x00
#define LCD_DISPLAY_ON 0b00000100
#define LCD_DISPLAY_NOCURSOR 0x00
#define LCD_DISPLAY_NOBLINK 0x00

#define LCD_CMD_CLEAR 0x01

#define LCD_CMD_ENTRY 0b00000100
#define LCD_ENTRY_INC 0b00000010
#define LCD_ENTRY_MOVE_CURSOR 0x00

#define LCD_CMD_SET_DDRAM 0b10000000
#define LCD_SET_LINE1 0x00
#define LCD_SET_LINE2 0x40

#define LCD_MAX_BUF_SIZE 32

// Function prototypes
void configureLCD(void);			// Configure the LCD module
void LCDdata(unsigned char cx);		// Write an 8-bit value to the LCD
void LCDcmd(unsigned char cx);		// Write an 8-bit command to the LCD
void LCDputs(unsigned char *str);	// Writes a string to the LCD
void LCDprintf(char* format, ...);	// Printf for the LCD module