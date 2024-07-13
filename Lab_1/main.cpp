/*
 * lab1_2020.cpp
 *
 * Created: 1/9/2020 Based on Lab_1_2019
 * Author : WWClark
 */ 

/*
This program is a solution to Lab 1 -- 
	Pushbutton switches are connected to Pins PD3 and PD4 (pulled high when switch is open).
	LEDs are connected to Pins PD6 and PD7 (active low). 
	
	The program causes the LEDs to mimic the input pin states (i.e. when Pin PD3 is high, the LED connected to Pin PD6 is on.  Likewise for Pins PD4 and PD7).

*/

#include <avr/io.h>

int main(void)
{
	 	
	DDRD = 0b11000000;  // set bits PD7-PD6 as output, PD5-PD0 as input (this covers Pins 3 and 4 as needed for inputs, and 6 and 7 as needed for outputs)
		// Follows a practice of making unused pins to be input (safer)
	PORTD = 0b11000000; // set output bits on PORTD so that all outputs are off (they are wired as active low)
		 
    // ============================================
    // P R O G R A M L O O P
    // ============================================
	while(1)
    {
				
       	if (PIND & 0b00001000) // Check the status of Switch 1.  
		   // This line checks the switch status by doing a Boolean operation between PIND (where the switch is connected) and a binary number that targets the 
		   // single pin that we are trying to read (Pin 3). For more on Boolean operations and "single bit addressing", see the C Programming slides.
		   // The argument of the If is TRUE only if the switch is closed (and Pin3 is a high voltage), in which case we want to turn on LED 1.
		{
			PORTD = PORTD & 0b10111111; // Turn on LED 1. 
			// This line uses more Boolean arithmetic to clear pin PD6 while leaving the other bits on PORTD unchanged.  Specifically, the line computes
			// a logical AND between the existing PORTD values and a binary number that has 0 in the only bit that we want to ensure is 0, PD6.  All other bits remain
			// unchanged (i.e. this line does not affect bit PD7, the other LED).
			// The result is that since the LED on pin 6 of PORTD is wired as active low, this line causes it to turn on.
		}
		else 
		{
			PORTD = PORTD | 0b01000000;  // Turn off LED 1. 
			// More Boolean arithmetic to cause PD6 to go high (a logical OR is performed between data in PORTD and a binary number that targets pin PD6), 
			// which causes LED 1 to be off and other bits, specifically LED 2, are unchanged.
		}
		// Repeat for LED and Switch 2
		if (PIND & 0b00010000) // Check the status of Switch 2.
			// Same as above but pin PD4 is the target.
		{
			PORTD = PORTD & 0b01111111;  // Turn on LED 2.
			// Same as in first If but pin PD7 is the target.
		}
		else 
		{
			PORTD = PORTD | 0b10000000; // Turn off LED 2.
			// Same as in first Else but pin PD7 is the target.
		}
		
	}
	
return 0;	
}
// ============================================
// S U B R O U T I N E S
// ============================================



