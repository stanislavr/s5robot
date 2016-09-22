//;******************************************************************************
//; spi.h - Header file for HCS12 SPI module
//:
//; Name: Thomas Abdallah
//; Date: 2016-04-01
//;
//;******************************************************************************

#define SPIBR_1MHz 0x02	// Baud rate divisor for SPIBR_1MHz
#define SPICR1_INIT 0b01010000	// Interrupts disabled, System Enabled, Master Mode, Active-high clocks, odd edges sampling, SS not enabled, MSB first
#define SPISR_INIT 0x00	// Default settings for Status Register

#define DAC_ON_CTRL 0b11010000	// Control bits (upper nibble) to power on the DAC
#define DAC_SET_CTRL_ALL 0b11110000	// Control bits (upper nibble) to load new values for DAC OP on both channels
#define DAC_SET_CTRL_A 0b10010000	// Control bits (upper nibble) to load new values for DAC OP on Channel A
#define DAC_SET_CTRL_B 0b10100000	// Control bits (upper nibble) to load new values for DAC OP on Channel B
#define DAC_VREF0 0b00000000	// Data bits to set vRef = 1.214V
#define DAC_VREF1 0b01000000	// Data bits to set vRef = 1.940V
#define DAC_VREF2 0b10000000	// Data bits to set vRef = 2.425V
#define DAC_VREF3 0b11000000	// Data bits to set vRef = 3.885V
#define DAC_SUB 0b00000000		// Sub bits for all DAC SPI functionality (lower nibble)

#define SHIFT_NIBBLE 0x04u		// To shift one nibble either way

// Function prototypes
void configureSPI(void);		// Configures the SPI module
void putcSPI(unsigned char cx);	// Output a character to the SPI port
void configureDAC(void);		// Configures the MAX5512 DAC for operation
void writeDAC(unsigned char data, unsigned char chnl);	// Set the DAC output voltage based on Digital data