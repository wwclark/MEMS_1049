/*
 * blink_using_timer_register.c
 *
 * Created: 5/19/2024
 * Author : WW Clark
 */
 
/* 
 *
  This program blinks an LED at a defined frequency.  Connect LED to PORTC0.
  
  Uses TIMER0 register at a single blink rate with a fixed timer prescaler.  
    
  Connect LED signal to oscilloscope to test frequencies.
*
 */ 

#include <avr/io.h>

#define FREQ_CLK 1000000 // Clock frequency (default setting for the ATmega328P -- 8 MHz internal clock with divide-by-8

//** Functions **//
void wait(volatile int multiple);

//** GLOBAL VARIABLES **/
// none

int main(void)
{
	// Setup
	DDRC = 0xFF; // Sets all pins of PORTC to output
	PORTC = 0xFF; // Turn off all pins on PORTC (assume pins are configured as active low)

   while(1)
    {
         // The main loop -- toggle the bit, then delay to set the frequency
         // Three different methods are shown for turning the LED on and off
         
         // Full Port Addressing
         // This block of code does the blinking by addressing the whole port (not the single PC0 bit)
         //PORTC = 0b00000001; // Turn off the LED -- note that this also clears all other bits on Port C
         //wait(250);
         //PORTC = 0b00000000; // Turn on the LED -- note that this also clears all other bits on Port C
         
         
         // Single Bit Addressing
         // This block of code does the blinking by addressing only the single PC0 bit
         PORTC = PORTC | 0b00000001; // Turn off the LED -- note that this only sets the PC0 bit and leaves the others unchanged
         wait(250);
         PORTC = PORTC & 0b11111110; // Turn on the LED -- note that this only clears the PC0 bit and leaves the others unchanged
         
         //Toggling the Bit
         // This block of code uses single bit addressing and additional Boolean arithmetic to toggle the LED state
         //PORTC = PORTC ^ 1<<PORTC0;  // Note that this uses the Exclusive OR operator, which toggles an individual bit without changing the other bits in PORTC
         
         wait(2500); // call delay function where the argument is number of msec to wait: the wait function will n times, each time creating a 1 msec delay
         
    }
	
	return(0);
} // end main()

void wait(volatile int number_of_msec) {
	// This subroutine creates a delay equal to number_of_msec*T, where T is 1 msec
	// It changes depending on the frequency defined by FREQ_CLK
	char register_B_setting;
	char count_limit;
	
	// Some typical clock frequencies: 
	switch(FREQ_CLK) {
		case 16000000:
			register_B_setting = 0b00000011; // this will start the timer in Normal mode with prescaler of 64 (CS02 = 0, CS01 = CS00 = 1). 
			count_limit = 250; // For prescaler of 64, a count of 250 will require 1 msec
			break;
		case 8000000:
			register_B_setting =  0b00000011; // this will start the timer in Normal mode with prescaler of 64 (CS02 = 0, CS01 = CS00 = 1). 
			count_limit = 125; // for prescaler of 64, a count of 125 will require 1 msec
			break;
		case 1000000:
			register_B_setting = 0b00000010; // this will start the timer in Normal mode with prescaler of 8 (CS02 = 0, CS01 = 1, CS00 = 0). 
			count_limit = 125; // for prescaler of 8, a count of 125 will require 1 msec
			break;
	}
	
	while (number_of_msec > 0) {
		TCCR0A = 0x00; // clears WGM00 and WGM01 (bits 0 and 1) to ensure Timer/Counter is in normal mode.
		TCNT0 = 0;  // preload value for testing on count = 250
		TCCR0B =  register_B_setting;  // Start TIMER0 with the settings defined above
		while (TCNT0 < count_limit); // exits when count = the required limit for a 1 msec delay
		TCCR0B = 0x00; // Stop TIMER0
		number_of_msec--;
	}
} // end wait()
