/*
 * Asynchronous_Serial_Transmitter.cpp
 *
 * Created: 3/21/2020 1:47:43 PM
 * Author : Buddy
 */ 
// This program is an example code for transmitting data using the USART for asynchronous serial communication on the ATmega328P
// After setting up the USART, the main function repeatedly sends a character.

#include <avr/io.h>

void initialize_usart(void); // function to set up USART
void transmit_data_usart(int); // function to transmit a byte by USART

int main(void)
{
	initialize_usart(); // Initialize the USART with desired parameters
	while(1) {
		transmit_data_usart(0x42);
		// Transmit desired character repeatedly – which is 0x42 = the ASCII code for the letter B in this case
	}
	return 0;
}

void initialize_usart(void) // function to set up USART
{
	UCSR0B = (1<<TXEN0); // enable serial transmission
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00) ; // Asynchronous mode, 8-bit data; no parity; 1 stop bit
	UBRR0L = 0x67; // 9,600 baud if Fosc = 16MHz
}

void transmit_data_usart(int data) // Function to transmit a character
{
	while (!(UCSR0A & (1<<UDRE0))); // Poll to make sure transmit buffer is ready, 				// then send data
	UDR0 = data;
}