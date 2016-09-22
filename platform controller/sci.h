//;******************************************************************************
//; SCI.h - Header file for SCI functions
//:
//; Name: Thomas Abdallah
//; Date: 2016-03-31
//;
//;******************************************************************************

#define sciBufSz 20u       // Number of elements for SCI Rx ring buffer

#define SCIBD_INIT 52	// For 9600 Baud
#define SCICR1_INIT 0x00	// For 1 start-bit, 8 data-bits, 1 stop-bit
//#define SCICR2_INIT 0b00101100

// RS232 Communication Protocol
#define msgStart '<'	// Start of a valid message
#define msgEnd '>'		// End of a valid message
#define NAK 0x49
#define ACK 0x53
#define HB 0x48
#define bufEmpty 0x00

// Function prototypes
void configureSCI(void);			// Configure the SCI module with parameters above
void putcSCI(unsigned char cx);		// Write a single character over SCI
void putsSCI(unsigned char *str);	// Write a string over SCI
unsigned char getcSCI(void);		// Read a single character over SCI
unsigned char consumeSCI(void);		// Consume the oldest thing from the SCI buffer