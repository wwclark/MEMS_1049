/*
 * stepper_example.c
 *
 * Created: 3/8/2021 1:42:42 PM
 * Author : wwclark
 */ 

// This program applies full wave stepping to a bi-polar stepper motor.
//  The motor will take two steps forward, one step back, in an infinite loop.

#include <avr/io.h>

#define FREQ_CLK 1000000 // Clock frequency (default setting for the ATmega328P -- 8 MHz internal clock with divide-by-8

// Functions
void step_CW( void );
void step_CCW( void );
void wait(volatile int multiple);

//** GLOBAL VARIABLES **
int phase_step = 1; // We will use wave stepping, so the steps are numbered 1-4, then will repeat.  

int main(void)
{
   // Setup
	DDRD = 0b11110000; // Sets pins 4, 5, 6, and 7 output, and 	pins 0, 1, 2 and 3 to input. Pins 4-7 will be used for 	stepper motor control
   while(1)
   {
	// Step forward two steps
	step_CW(); //step_CW()
	wait(250); // delay so that motor doesn't skip steps
	step_CW(); //step_CW()
	wait(250); // delay so that motor doesn't skip steps

	// Step backward one step
	step_CCW(); // step_CCW()
	wait(250); // delay so that motor doesn't skip steps
  
   } // end main while

}

void step_CCW()
{
	// This function advances the motor counter-clockwise one step.  Follow the full-wave stepping table in Stepper Motor Control.ppt for MEMS 1049 course. phase1a = PORTD_7, phase1b = PORTD_6
	// phase2a = PORTD_5, phase2b = PORTD_4

	switch (phase_step) {
		case 1:
		// step to 2
		PORTD = 0b00010000;
		phase_step = 2;
		break;
		case 2:
		// step to 3
		PORTD = 0b01000000;
		phase_step = 3;
		break;
		case 3:
		// step to 4;
		PORTD = 0b00100000;
		phase_step = 4;
		break;
		case 4:
		// step to 1;
		PORTD = 0b10000000;
		phase_step = 1;
	break;  }
}// end step_CCW

void step_CW() {
	// This function advances the motor clockwise one step.  Follow the full-wave stepping table in Stepper Motor Control.ppt for MEMS 1049 course. Our stepper motor phases are assigned to Port pins as follows: phase1a = PORTD_7, phase1b = PORTD_6, phase2a = PORTD_5, phase2b = PORTD_4

	switch (phase_step) {
		case 1:
		// step to 4
		PORTD = 0b00100000;
		phase_step = 4;
		break;
		case 2:
		// step to 1
		PORTD = 0b10000000;
		phase_step = 1;
		break;
		case 3:
		// step to 2;
		PORTD = 0b00010000;
		phase_step = 2;
		break;
		case 4:
		// step to 3;
		PORTD = 0b01000000;
		phase_step = 3;
	break;  }
}// end step_CW

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
