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

//** Functions **//
void wait(int msec);
void delayNms_timer0();

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

void wait(int multiple) {
	//*** wait ***
	/* This subroutine calls others to create a delay.
		 Total delay = multiple*N, where N is in msec and is the delay created by the called function.
	
		Inputs: multiple = number of multiples to delay, where multiple are the number of items an actual delay loop is called.
		Outputs: None
	*/
	
	while (multiple > 0) {
		delayNms_timer0();
		multiple = multiple - 1;
	}
} // end wait()

void delayNms_timer0() {
    //*** delay N ms **
    /* This subroutine creates a delay of N msec using TIMER0 with prescaler on clock, where, for a 16MHz clock:
    		N = .0156 msec for no prescaler and count of 250 (preload counter with 5)
    		N = 0.125 msec for prescaler set to 8 and count of 250 (preload counter with 5)
    		N = 1 msec for prescaler set to 64 and count of 250 (preload counter with 5)
    		N = 4 msec for prescaler set to 256 and count of 250 (preload counter with 5)
    		N = 16 msec for prescaler set to 1,024 and count of 250 (preload counter with 5)
	
			Inputs: None
			Outputs: None
	*/
	
	TCCR0A = 0x00; // clears WGM00 and WGM01 (bits 0 and 1) to ensure Timer/Counter is in normal mode.
	
	TCNT0 = 5;  // preload load TIMER0  (count must reach 255-5)
	
	//TCCR0B = 1<<CS00;				//TCCR0B = 0x01; Start TIMER0, Normal mode, crystal clock, no prescaler
	// TCCR0B = 1<<CS01;			//TCCR0B = 0x02; // Start TIMER0, Normal mode, crystal clock, prescaler = 8
	TCCR0B =  1<<CS01 | 1<<CS00;	//TCCR0B = 0x03;  // Start TIMER0, Normal mode, crystal clock, prescaler = 64
	//TCCR0B = 1<<CS02;				//TCCR0B = 0x04; // Start TIMER0, Normal mode, crystal clock, prescaler = 256
	//TCCR0B = 1<<CS02 | 1<<CS00;	//TCCR0B = 0x05; // Start TIMER0, Normal mode, crystal clock, prescaler = 1024
	
	while ((TIFR0 & (0x1<<TOV0)) == 0); // wait for TOV0 to roll over:
	// How does this while loop work?? See notes
	
	TCCR0B = 0x00; // Stop TIMER0
	TIFR0 = 0x1<<TOV0;  // Clear TOV0 (note that this is an odd bit in that it
	//is cleared by writing a 1 to it)
} // end delayNms_timer0()