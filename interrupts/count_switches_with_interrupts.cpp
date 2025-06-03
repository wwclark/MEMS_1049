/*
 * count_switches_with_interrupts.cpp
 *
 * Created: 1-26-2020
 * Author : Buddy
 */ 

/*
This program modifies the count_switches_with_raw_count_debounce.cpp code so that the monitoring of switches is 
	done with interrupts so that the microcontroller doesn't need to continuously poll the switches.
	
	Pushbutton switches are connected to Pins PD2 and PD3 (changed from previous code so that they are connected 
	to interrupt pins).  The switches are configured such that they are pulled high when switch is open.
	
	LEDs are connected to Pins PC2-PC3 (active low). This is different from previous example so that it can be 
	tested on Lab 3 hardware. 
	
	The LEDs are interpreted to mimic the input pin states (i.e. when Pin PD2 
	is high, the LED connected to Pin PC2 is to be on.  Likewise for Pins PD3 and PC3).
	This is to be the same for the switches on doors: the LEDs are to be on when the door is open, 
	and off when the door is closed.  
	
	Door openings are counted, so switch debouncing is used (in this case, using the Timer0 counter) to prevent 
	inadvertent counts.


*/

#include <avr/io.h>
#include <avr/interrupt.h>

#define FREQ_CLK 1000000 // Clock frequency (default setting for the ATmega328P -- 8 MHz internal clock with divide-by-8

//** GLOBAL VARIABLES **/
int count_door1 = 0;
int count_door2 = 0;
volatile int i = 0; 
int debounce = 50; // number of msec for debouncing the switches

//** FUNCTION DECLARATIONS **//
void wait(volatile int multiple);

int main(void)
{
	DDRD = 0b00000000;  // set bits of PORTD as input (only need PD2 and PD3 as input for the interrupts)
	DDRC = 0b11111111; // set bits PORTC as output (only need PC2 and PC3 as output to control the LEDs)
	PORTC = 0b11111111; // set all bits of PORTC high to turn off LEDs (assumes circuit is wired as active low)
	
	// Set up Interrupts
	EICRA = 1<<ISC01 | 1<<ISC00 | 1<<ISC11 | 1<<ISC10;	// Trigger INT0 and INT1 on rising edge
	EIMSK = 1<<INT1 | 1<<INT0;	// Enable INT1 and INT0
	sei();	//Enable Global Interrupt
		 
// todo: Add code at the beginning that waits for the doors to be closed before counting begins.  In the current 
// code, it is assumed that the program begins while the doors are in a closed state. Otherrwise, the count may be out 
// of sync with doors.

   	while(1) // The main loop does nothing, because all activity is in the interrupt routines. This loop could be used to control other things.      
    {
		i = 1; // a do-nothing statement to preserve the empty while loop
	}
	
return 0;	
} // end main



//Interrupt Service Routine for INT0
ISR(INT0_vect) // This routine is entered if Switch 1 goes high (door 1 is opened)
{
	PORTC = PORTC & 0b11111011; // Turn on LED by clearing PC2 (which is active low),  leave other bits unchanged  (AND PORTC with a binary number that has 0 in the only bit that we want to ensure is 0)
	count_door1++; // Increment count on door opening
	wait(debounce); // debounce the switch
	
	while ((PIND & 0b00000100)) // wait for PD2 to go low again (wait for door to close). "Read" the contents of PortD2 using bit-wise operation (AND PIND with binary 4 so that only PD2 is read).  Exit the loop when it is 0.
	{
		i=0; // a do-nothing statement to preserve the empty while loop
	}
	PORTC = PORTC | 0b00000100;  // Turn off LED by setting bit PC2 (which is active low), leave other bits unchanged (OR PORTC with a binary number that has 1 in the only bit that we want to ensure is 1)
	wait(debounce); // debounce the switch
	EIFR = 0b00000001;  // Clear the INT0 interrupt flag in case it was reset while in the ISR (so that it doesn't run again) but ignore the INT1 flag in case it was set while in this ISR
	
}

ISR(INT1_vect)
{
	PORTC = PORTC & 0b11110111; // Turn on LED by clearing PC3 (which is active low),  leave other bits unchanged  (AND PORTC with a binary number that has 0 in the only bit that we want to ensure is 0)
	count_door2++; // Increment count on door opening
	wait(debounce); // debounce the switch
	
	while ((PIND & 0b00001000)) // wait for PD3 to go low again (wait for door to close). "Read" the contents of PortD3 using bit-wise operation (AND PIND with binary 8 so that only PD2 is read).  Exit the loop when it is 0.
	{
		i=0; // a do-nothing statement to preserve the empty while loop
	}
	
	PORTC = PORTC | 0b00001000;  // Turn off LED by setting bit PC3 (which is active low), leave other bits unchanged (OR PORTC with a binary number that has 1 in the only bit that we want to ensure is 1)
	wait(debounce); // debounce the switch
	EIFR = 0b00000010;  // Clear the INT1 interrupt flag in case it was reset while in the ISR (so that it doesn't run again) but ignore the INT0 flag in case it was set while in this ISR	
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