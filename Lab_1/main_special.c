/*
 * Lab1_Special.c
 *
 * Created: 1/16/2024 Based on Lab_1_2021
 * Author : WWClark
 */ 

/*
This program is a solution to Lab 1 -- 
	Pushbutton switches are connected to Pins PD3 and PD4 (pulled high when switch is open).
	LEDs are connected to Pins PD6 and PD7 (active low). 
	
	The program causes the LEDs to mimic the input pin states (i.e. when Pin PD3 is high, the LED connected to Pin PD6 is on.  Likewise for Pins PD4 and PD7).

	Extra code is added to accept the special code for releasing box latch.
*/

#include <avr/io.h>


//** Functions **//
void wait(volatile int multiple);
int wait_for_switch_cycle(void);

int main(void)
{
	 	
	DDRD = 0xE0;  // set bits PD7-PD5 as output, PD4-PD0 as input (this covers Pins 3 and 4 as needed for inputs, and 6 and 7 as needed for outputs)
	PORTD = 0b11111111; // set all bits on PORTD so that all output are off (assumes circuit is wired as active low)
	DDRB = 0xFF;  // set all PortB bits as output.  We'll use PB1 to control the door latch.
	PORTB = 0b00000000; // set all bits on PORTB to zero 
		 
    // ============================================
    // P R O G R A M L O O P
    // ============================================
	
	while(1) 
	{		
		
		// create a blink cycle that indicates an error occurred in the cycle and the system is ready to try again
		// also, this is a way to check the clock frequency 
		for(int i=0; i<3; i++) // switch press sequence loop
		{
			// Turn off LEDs
			PORTD = PORTD | 0b01000000; 
			PORTD = PORTD | 0b10000000; 
			wait(250);
			PORTD = PORTD & 0b10111111;
			PORTD = PORTD & 0b01111111; 
			wait(250);
		}
		
		int result = 0;	
       	while(1) // this loop continues as long as correct presses a made; an incorrect press breaks the loop and starts it again
		   {
				result = wait_for_switch_cycle(); // first S1
				if (result == 2) // This means that S2 was incorrectly pressed
				{
					break;
				}
				result = wait_for_switch_cycle(); // second S1
				if (result == 2) // This means that S2 was incorrectly pressed
				{
					break;
				}
				result = wait_for_switch_cycle(); // first S2
				if (result == 1) // This means that S1 was incorrectly pressed
				{
					break;
				}
				result = wait_for_switch_cycle(); // second S2
				if (result == 1) // This means that S1 was incorrectly pressed
				{
					break;
				}
				result = wait_for_switch_cycle(); // third S2
				if (result == 1) // This means that S1 was incorrectly pressed
				{
					break;
				}
				result = wait_for_switch_cycle(); // third S1
				if (result == 2) // This means that S2 was incorrectly pressed
				{
					break;
				}
				result = wait_for_switch_cycle(); // fourth S1
				if (result == 2) // This means that S2 was incorrectly pressed
				{
					break;
				}
				// If you made it here, success!
				// Open the latch.
				PORTB = PORTB | 0b00000010; // Set PB1 to open the latch
				wait(3000); // Hold the latch open for 5 seconds
				PORTB = PORTB & 0b00000000; // Reset PB1
							   
       	} // end switch press sequence loop
		   
	} //end program loop
	
return 0;	
}
// ============================================
// S U B R O U T I N E S
// ============================================

int wait_for_switch_cycle(void) {
	/* This function waits for a switch to be pressed and when it does, it:
		i) sets the proper LED
		ii) returns switch state
			- action = 1 if switch S1 has been pressed and released
			- action = 2 if switch S2 has ee pressed and released
			*/
	int action = 0;
	char wait_for_release = 1;
	while(wait_for_release)	
	{
				if (PIND & 0b00001000) // "Read" the contents of PortD3 using bit-wise operation (AND PIND with binary 8 so that only PD3 is read)
				// When read  as 1 (open switch) clear bit PD6 to turn on the LED.
				{
					PORTD = PORTD & 0b10111111; // Clear PD6 (which is active low),  leave other bits unchanged  (AND PIND with a binary number that has 0 in the only bit that we want to ensure is 0)
				}
				else
				{
					PORTD = PORTD | 0b01000000;  // Switch S1 has been pressed
					// Set bit PD6 to turn off the  LED (which is active low), leave other bits unchanged (OR PIND with a binary number that has 1 in the only bit that we want to ensure is 1)
					wait(50); // switch debounce
					while(wait_for_release)
					{
						if (PIND & 0b00001000)
						{
							wait(50); // switch debounce
							action = 1; // This ends cycle on switch S1
							wait_for_release = 0;
						}
					}
				}
				
				if (PIND & 0b00010000) // "Read" the contents of PortD4 using bit-wise operation (AND PIND with binary 16 so that only PD4 is read)
				{
					PORTD = PORTD & 0b01111111;  // Clear PD7, leave other bits unchanged (AND PIND with a binary number that has 0 in the only bit that we want to ensure is 0)
				}
				else
				{
					PORTD = PORTD | 0b10000000; // Switch S2 has been pressed
					// Set PD7, leave other bits unchanged (OR PIND with a binary number that has 1 in the only bit that we want to ensure is 1)
					wait(50); // switch debounce
					while(wait_for_release)
					{
						if (PIND & 0b00010000)
						{
							wait(50); // switch debounce
							action = 2; // This ends cycle on switch S2
							wait_for_release = 0;
						}
					}
				}

	}
	return(action);
}
	
	
void wait(volatile int N) {
	// This subroutine creates a delay equal to N msec
	while (N > 0) {
		TCCR0A = 0x00; // clears WGM00 and WGM01 (bits 0 and 1) to ensure Timer/Counter is in normal mode.
		TCNT0 = 0;  // preload value for testing on count = 250
		TCCR0B =  0b00000011; //1<<CS01 | 1<<CS00;	TCCR0B = 0x03;  // Start TIMER0, Normal mode, 16 MHz crystal clock, prescaler = 64
		while (TCNT0 < 0xFA); // exits when count = 250 (which is the count required to create a 1 msec delay with 16MHz clock and prescaler of 64)
		TCCR0B = 0x00; // Stop TIMER0
		N--;
	}
} // end wait()
