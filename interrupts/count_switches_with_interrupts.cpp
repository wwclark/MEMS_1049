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

//** GLOBAL VARIABLES **/
int count_door1 = 0;
int count_door2 = 0;
volatile int i = 0; 
int debounce = 50; // number of msec for debouncing the switches

//** FUNCTION DECLARATIONS **//
void wait(volatile int multiple, volatile char time_choice);
void delay_T_msec_timer0(char choice);

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
	wait(debounce,2); // debounce the switch
	
	while ((PIND & 0b00000100)) // wait for PD2 to go low again (wait for door to close). "Read" the contents of PortD2 using bit-wise operation (AND PIND with binary 4 so that only PD2 is read).  Exit the loop when it is 0.
	{
		i=0; // a do-nothing statement to preserve the empty while loop
	}
	PORTC = PORTC | 0b00000100;  // Turn off LED by setting bit PC2 (which is active low), leave other bits unchanged (OR PORTC with a binary number that has 1 in the only bit that we want to ensure is 1)
	wait(debounce,2); // debounce the switch
	EIFR = 0b00000001;  // Clear the INT0 interrupt flag in case it was reset while in the ISR (so that it doesn't run again) but ignore the INT1 flag in case it was set while in this ISR
	
}

ISR(INT1_vect)
{
	PORTC = PORTC & 0b11110111; // Turn on LED by clearing PC3 (which is active low),  leave other bits unchanged  (AND PORTC with a binary number that has 0 in the only bit that we want to ensure is 0)
	count_door2++; // Increment count on door opening
	wait(debounce,2); // debounce the switch
	
	while ((PIND & 0b00001000)) // wait for PD3 to go low again (wait for door to close). "Read" the contents of PortD3 using bit-wise operation (AND PIND with binary 8 so that only PD2 is read).  Exit the loop when it is 0.
	{
		i=0; // a do-nothing statement to preserve the empty while loop
	}
	
	PORTC = PORTC | 0b00001000;  // Turn off LED by setting bit PC3 (which is active low), leave other bits unchanged (OR PORTC with a binary number that has 1 in the only bit that we want to ensure is 1)
	wait(debounce,2); // debounce the switch
	EIFR = 0b00000010;  // Clear the INT1 interrupt flag in case it was reset while in the ISR (so that it doesn't run again) but ignore the INT0 flag in case it was set while in this ISR	
}



void wait(volatile int multiple, volatile char time_choice) {
	//*** wait ***
	/* This subroutine calls others to create a delay.
		 Total delay = multiple*T, where T is in msec and is the delay created by the called function.
	
		Inputs: multiple = number of multiples to delay, where multiple is the number of times an actual delay loop is called.
		Outputs: None
	*/
	
	while (multiple > 0) {
		delay_T_msec_timer0(time_choice); // we are choosing case 2, which is a 1 msec delay
		multiple--;
	}
} // end wait()

void delay_T_msec_timer0(char choice) {
    //*** delay T ms **
    /* This subroutine creates a delay of T msec using TIMER0 with prescaler on clock, where, for a 16MHz clock:
    		for Choice = 1: T = 0.125 msec for prescaler set to 8 and count of 250 (preload counter with 5)
    		for Choice = 2: T = 1 msec for prescaler set to 64 and count of 250 (preload counter with 5)
    		for Choice = 3: T = 4 msec for prescaler set to 256 and count of 250 (preload counter with 5)
    		for Choice = 4: T = 16 msec for prescaler set to 1,024 and count of 250 (preload counter with 5)
			for Choice = Default: T = .0156 msec for no prescaler and count of 250 (preload counter with 5)
			
			Inputs: None
			Outputs: None
	*/
	
	TCCR0A = 0x00; // clears WGM00 and WGM01 (bits 0 and 1) to ensure Timer/Counter is in normal mode.
	TCNT0 = 0;  // preload value for testing on count = 250
	// preload value for alternate test on while loop: TCNT0 = 5;  // preload load TIMER0  (count must reach 255-5 = 250)
	
	switch ( choice ) { // choose prescaler
		case 1:
			TCCR0B = 0b00000010; //1<<CS01;	TCCR0B = 0x02; // Start TIMER0, Normal mode, crystal clock, prescaler = 8
		break;
		case 2:
			TCCR0B =  0b00000011; //1<<CS01 | 1<<CS00;	TCCR0B = 0x03;  // Start TIMER0, Normal mode, crystal clock, prescaler = 64
		break;
		case 3:
			TCCR0B = 0b00000100; //1<<CS02;	TCCR0B = 0x04; // Start TIMER0, Normal mode, crystal clock, prescaler = 256
		break; 
		case 4:
			TCCR0B = 0b00000101; //1<<CS02 | 1<<CS00; TCCR0B = 0x05; // Start TIMER0, Normal mode, crystal clock, prescaler = 1024
		break;
		default:
			TCCR0B = 0b00000001; //1<<CS00; TCCR0B = 0x01; Start TIMER0, Normal mode, crystal clock, no prescaler
		break;
	}
	
	while (TCNT0 < 0xFA); // exits when count = 250 (requires preload of 0 to make count = 250)
	// alternate test on while loop: while ((TIFR0 & (0x1<<TOV0)) == 0); // wait for TOV0 to roll over:
	// How does this while loop work?? See notes
	
	TCCR0B = 0x00; // Stop TIMER0
	//TIFR0 = 0x1<<TOV0;  // Alternate while loop: Clear TOV0 (note that this is a nonintuitive bit in that it is cleared by writing a 1 to it)
	
} // end delay_T_msec_timer0()