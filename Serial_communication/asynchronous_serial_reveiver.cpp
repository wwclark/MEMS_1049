/*
 * Asynchronous_Serial_Reveiver.cpp
 *
 * Created: 3/21/2020 4:31:44 PM
 * Author : Buddy
 */ 

// This program receives a single byte by way of Asynchronous Serial Communication.

#include <avr/io.h>


//** Functions **//
void initialize_usart(); // setup function for USART

int main(void)
{
	// Setup
	int data; // variable with which to receive incoming data
	
	// Initialize the USART with desired parameters   (this will override PD0 and PD1 digital output functions)
	initialize_usart();
	
	while(1) {
		while (!(UCSR0A & 0b10000000)); // ((! (UCSR0A & (1<<RXC0)));  // Wait until new data arrives
		data =  UDR0; // read the data
	} // end main while
	return 0;
} // end main


void initialize_usart(void) // setup function for USART
{
	UCSR0B = 0b00010000; //(1<<RXEN0); // enable serial transmission
	UCSR0C = 0b00000110; //(1<<UCSZ01) | (1<<UCSZ00); // Asynchronous mode, 8-bit data; no parity; 1 stop bit
	UBRR0L = 0x67; // 9,600 baud if Fosc = 16MHz
}