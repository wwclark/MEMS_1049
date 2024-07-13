/*
 * asynch_serial_loopback_test.cpp
 *
 * Created: 3/29/2020
 * Author : WW Clark
 */ 


/* This program does a loop back test on the asnychronous serial communication lines on the ATmega328P.

A byte is transmitted using asynchronous serial communication out the Tx line.  It is also read in through the Rx line on the same Atmega chip, then displayed on PORTB pins.  
To test, leave Rx pin disconnected.  Turn on the microcontroller with this program uploaded.  Nothing should display on PORTB pins since the data is not looped back.
Then connect Rx to Tx, and the data should appear.  

You can change the data value and repeat and the new data should appear.
*/

#include <avr/io.h>
//** Functions **//
void wait(int msec);
void delayNms_timer0();
void initialize_usart(); // setup function for USART
void transmit_data_usart(int); // Function to transmit a character
int main(void)
{
// Setup
	
	DDRB = 0xFF;  // define all Port B bits as output for display of loop back data:
	PORTB = 0xFF;

	// Initialize the USART with desired parameters
	 initialize_usart(); 
	 
  while(1)
    {
  	transmit_data_usart(0x37); 	
	// test loop back -- read the value from Rx line
	// Wait until loop back data arrives 
	wait(250);
	PORTB =  UDR0;  // display the received value on LEDs
	} // end main while
	return 0;
} // end main
void initialize_usart(void) // setup function for USART
{
	UCSR0B = (1<<TXEN0) | (1<<RXEN0); // enable serial transmission; 	UCSR0B = (1<<RXEN0); // enable serial receive
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); // Asynchronous mode, 8-bit data; no parity; 1 stop bit
	UBRR0L = 0x67; // 9,600 baud if Fosc = 16MHz
}
void transmit_data_usart(int data_out) // Function to transmit a character
{
	while (!(UCSR0A & (1<<UDRE0))); // Poll to make sure transmit buffer is ready, then send data
	UDR0 = data_out;
}
void wait(int msec) 
{
	// This subroutine calls others to create a delay
	// function passes number of msec to delay
	while (msec > 0) {
		delayNms_timer0();
		msec = msec - 1;
	}
} // end wait
void delayNms_timer0() 
{
	// This subroutine creates a delay of N msec using TIMER0 with prescaler on clock, where, for a 16MHz clock:
	//		N = .0156 msec for no prescaler and count of 250 (preload counter with 5)
	//		N = 0.125 msec for prescaler set to 8 and count of 250 (preload counter with 5)
	//		N = 1 msec for prescaler set to 64 and count of 250 (preload counter with 5)
	//		N = 4 msec for prescaler set to 256 and count of 250 (preload counter with 5)
	//		N = 16 msec for prescaler set to 1,024 and count of 250 (preload counter with 5)

	TCCR0A = 0x00; // clears WGM00 and WGM01 (bits 0 and 1) to ensure Timer/Counter is in normal mode.
	TCNT0 = 5;  // load timer0 with number for delay (count must reach 255-5)
	//TCCR0B = 0x01; // Start Timer0, Normal mode, crystal clock, no prescaler
	// TCCR0B = 0x02; // Start Timer0, Normal mode, crystal clock, prescaler = 8
	TCCR0B =  0x03;  // Start Timer0, Normal mode, crystal clock, prescaler = 64
	//TCCR0B = 0x04; // Start Timer0, Normal mode, crystal clock, prescaler = 256
	//TCCR0B = 0x05; // Start Timer0, Normal mode, crystal clock, prescaler = 1024

	while ((TIFR0 & (0x1<<TOV0)) == 0); // wait for TOV0 to roll over:

	TCCR0B = 0x00; // Stop Timer0
	TIFR0 = 0x1<<TOV0;  // Clear TOV0 (note that this is an odd bit in that it is cleared by writing a 1 to it)
}



