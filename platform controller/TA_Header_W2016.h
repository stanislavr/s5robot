//;******************************************************************************
//; TA_Header_W2016 - Header file with various macros for the HCS12 project
//:
//; Name: Thomas Abdallah
//; Date: 2016-03-29
//;
//;******************************************************************************

//;******************************************************************************
//;	Macros
//;******************************************************************************

// Mask off the lower byte of a specified value
#define LOW(value) ((value) & 0xFF)

// Evaluates to the low nybble of the argument
#define LO_NYBBLE(value) (0x0F & (value))

// Evaluates to the upper nybble of the argument
#define HI_NYBBLE(value) LO_NYBBLE((value)>>4)

// Clear mask bits
#define CLEAR_BITS(port, mask) ((port) &= LOW(~(mask)))

// Set mask bits
#define SET_BITS(port, mask) ((port) |= (mask))

// Toggle mask bits
#define TOGGLE_BITS(port, mask) ((port) ^= (mask))

// Force a subset of bits in any register to a specific value
#define FORCE_BITS(reg, bits, mask) ((reg) = ((reg) & LOW(~(bits))) | ((mask) & (bits)))

// Force a subset of bits in any 16-bit register tyo a specific value
#define FORCE_WORD(port, mask, value) (port) = ((port) & ((~(mask)) & 0xFFFF) | ((value) & (mask)))

// Configures PA7 as an output for enable/disable of 5VA supply on my board
#define CONFIGURE_5VA (SET_BITS(DDRA, Mask_5VA))

// Enable 5VA supply on my HCS12 board - Sets PA7
#define ENABLE_5VA (SET_BITS(PORTA, Mask_5VA))

// Disable 5VA supply on my HCS12 board - Clears PA7
#define DISABLE_5VA (CLEAR_BITS(PORTA, Mask_5VA))

// Configures PJ6 and PJ7 as outputs for SPI CS1 and CS2
#define CONFIGURE_SPI_CS (SET_BITS(DDRJ, (SPI_CS1|SPI_CS2)))

// Enable and Disable CS1 for DAC
#define ENABLE_CS_DAC (CLEAR_BITS(PTJ, SPI_CS1))
#define DISABLE_CS_DAC (SET_BITS(PTJ, SPI_CS1))

// Enable and CS2 for SPI expansion
#define ENABLE_CS_CS2 (CLEAR_BITS(PTJ, SPI_CS2))
#define DISABLE_CS_CS2 (SET_BITS(PTJ, SPI_CS2))

// Configure PS2 and PS3 as outputs to drive LEDs on my board
#define CONFIGURE_LEDS (SET_BITS(DDRS, (LED1|LED2)))
#define LED1_ON (SET_BITS(PTS, LED1))
#define LED2_ON (SET_BITS(PTS, LED2))
#define LED1_OFF (CLEAR_BITS(PTS, LED1))
#define LED2_OFF (CLEAR_BITS(PTS, LED2))

// Toggle LED1 and LED2 (PTS2 and PTS3)
#define TOGGLE_LEDS TOGGLE_BITS(PTS, (LED1|LED2))

// Enable a specified PWM channel
#define ENABLE_PWM_CHNL(chnl) SET_BITS(PWME, 1<<(chnl))

// Disable a specified PWM channel
#define DISABLE_PWM_CHNL(chnl) CLEAR_BITS(PWME, 1<<(chnl))

// Configure a specified timer channel as an output compare channel
#define MAKE_CHNL_OC(chnl) SET_BITS(TIOS, 1<<(chnl))

// Configure a specified timer channel as an input capture channel
#define MAKE_CHNL_IC(chnl) CLR_BITS(TIOS, 1<<(chnl))

// Clear flag bit for a specified timer channel
#define CLR_TIMER_CHNL_FLAG(chnl) TFLG1 = (1<<(chnl))

// Configure a specified timer channel to perform a specific action when the next OC event happens
#define SET_OC_ACTION(chnl, action) FORCE_WORD(TCTL_1_2, (0x03<<((chnl)*2)), ((action) << ((chnl)*2)))

// Configure a specified timer channel to perform a specific action and force an output compate on the next timer tick
#define FORCE_OC_ACTION_NOW(chnl, action) SET_OC_ACTION((chnl), (action)); SET_BITS(CFORC, (1<<(chnl)))

//;******************************************************************************
//;	Constants for Port naming/masking
//;******************************************************************************

// 5VA Enable Mask (PA7)
#define Mask_5VA 0b10000000

// SPI_CS Pins (for CS1 and CS2) on Port J
#define SPI_CS1 0b10000000
#define SPI_CS2 0b01000000

// LED Patterns & Masks (PS2 & PS3)
#define LED1 0b00000100 //Port S-2 as output
#define LED2 0b00001000 //Port S-3 as output