/*
 * interrupt_example.c
 *
 * Created: 3/14/2016 
 *  Author: WWClark
 */ 
/* This program is an example C code for implementing an interrupt using the Atmel chip.

Assume LEDs are attached to pins on PORTC, and they change by some pattern in the main program.  When an interrupt occurs, 
the interrupt routines do something, then the main code resumes with the pattern.

*/

#include <avr/io.h>
#include <avr/interrupt.h>

#define FREQ_CLK 1000000 // Clock frequency (default setting for the ATmega328P -- 8 MHz internal clock with divide-by-8

//** Functions **//
void wait(volatile int multiple);

//Interrupt Service Routine for INT0
ISR(INT0_vect)
{
	unsigned char temp;
	
	temp = PORTC;	// Save current value on PORTC -- could push this to the Stack
	
	PORTC = 0b11111111; // turn off all bits of PORTC
	
	wait(250);
		
	PORTC = temp;	//Restore old value to PORTC -- may first need to pull this from the Stack
}
ISR(INT1_vect)
{
	unsigned char temp;
	
	temp = PORTC;	// Save current value on PORTC -- could push this to the Stack
	
	PORTC = 0b00000000;  // Turn on all bits of PORTC
	
	wait(250);
	
	PORTC = temp;	//Restore old value to PORTC -- may first need to pull this from the Stack
	
}

int main(void)
{
	DDRD = 0<<PD2 | 0<<PD3;		// Set PD2 and PD3 as input (For use as interrupts INT0 and INT1)
	PORTD = 1<<PD2 | 1<<PD3;		// Enable PD2 and PD3 pull-up resistors
	DDRC = 0b11111111; // Set PORTC as output
		
	EICRA = 1<<ISC01 | 1<<ISC00 | 1<<ISC11 | 0<<ISC10;	// Trigger INT0 on rising edge; Trigger INT1 on falling edge
	
	EIMSK = 1<<INT1 | 1<<INT0;					// Enable INT1 and INT0
	
	sei();				//Enable Global Interrupt
	
	while(1)
	{
		PORTC = PORTC ^ 1;  // The main loop -- toggle bit 0 in PORTC
		wait(2);
	}
	
return(0);
}

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