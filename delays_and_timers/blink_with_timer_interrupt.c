/*
 * blink_with_timer_interrupt.c
 *
 * Created: 5/19/2024
 * Author : WW Clark
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
 
#define FREQ_CLK 1000000 // Clock frequency (default setting for the ATmega328P -- 8 MHz internal clock with divide-by-8

/* 
 *
  This program blinks an LED at set frequencies.  The timing is handled completely by the Timer0 register in the background, and the microcontroller can be doing some other task.
  Connect LED to PC0.
  
  Uses TIMER0 register and interrupt on overflow.
    
  Connect LED signal to oscilloscope to test frequencies.
*
 */ 

//** GLOBAL VARIABLES **/
volatile int loop = 0;
volatile int number_of_msec = 500; // this is 1/2 of the period of the blink rate in msec, which equals the number of times the ISR needs to run to cause the delay before changing the LED state
volatile char register_B_setting;
volatile char timer_preload;

//Interrupt Service Routine for Timer0
ISR(TIMER0_OVF_vect)
{
	loop++;
	if (loop==number_of_msec) { // if true, the desired time has elapsed
		// Timer keeps running but need to clear the interrupt flag, reset the loop counter, and toggle the LED
		TIFR0 = 0x1<<TOV0;  // Clear TOV0 (note that this is an odd bit in that it is cleared by writing a 1 to it)
		loop = 0; // reset count for another 1 second delay
		PORTC = PORTC ^ 1<<PORTC0;  // Toggle the LED //Note that this uses the Exclusive OR operator, which toggles an individual bit without changing the other bits in PORTC
	}
}

int main(void)
{
	int i = 0; // dummy variable
	
	// Set up ports
	DDRC = 0xFF; // Sets all pins of PORTC to output
	PORTC = 0xFF; // Turn off all pins on PORTC (assume the pins are wired as active low)
	// Set up timer
	// Some typical clock frequencies:
	switch(FREQ_CLK) {
		case 16000000:
			register_B_setting = 0b00000011; // this will start the timer in Normal mode with prescaler of 64 (CS02 = 0, CS01 = CS00 = 1).
			timer_preload = 255-250; // For prescaler of 64, a count of 250 will require 1 msec
			break;
		case 8000000:
			register_B_setting =  0b00000011; // this will start the timer in Normal mode with prescaler of 64 (CS02 = 0, CS01 = CS00 = 1).
			timer_preload = 255 - 125; // for prescaler of 64, a count of 125 will require 1 msec
			break;
		case 1000000:
			register_B_setting = 0b00000010; // this will start the timer in Normal mode with prescaler of 8 (CS02 = 0, CS01 = 1, CS00 = 0).
			timer_preload = 255- 125; // for prescaler of 8, a count of 125 will require 1 msec
			break;
	}
	
	TCCR0A = 0x00; // clears WGM00 and WGM01 (bits 0 and 1) to ensure Timer/Counter is in normal mode.
	TIMSK0 = 1<<TOIE0;	// Enable TIMER0 overflow interrupt
	TCNT0 = timer_preload;  // preload load TIMER0 
	sei();	//Enable Global Interrupt
	TCCR0B = register_B_setting;	//TCCR0B is set according to the clock frequency as defined in the switch case above.  This starts the timer.
		
   while(1)  // this is just a dummy loop that continually does some task while the timing and LED blinking is handled automatically in the background
    {
       i++;
	   if (i == 1000) {
		   i = 0;
	   }		          
    }
} // end main()

