/*
 * SPI_Main.cpp
 *
 * Created: 3/25/2018 10:06:32 AM
 * Author : Buddy
 
   This program sets up SPI in Main mode, mode 0, with SCK frequency of Fosc/16.
   Then the letter 'R' is transmitted.
   Whatever data that is received from Secondary is displayed on PortD
 ;
 
 */ 

#include <avr/io.h>


int main(void) {
	char data_in; // This variable will be used to hold incoming data
	
	// setup
	DDRD = 0xFF; // Set DDRD to display data that comes in from Secondary
	DDRB = 0b00101100; //	DDRB = 1<<PORTB5 | 1<<PORTB3 | 1<<PORTB2;  // Set pins SCK, MOSI, and SS as output

	// Set up Main SPI
	SPCR = 0b01010001; // SPCR = 1<<SPE | 1<<MSTR | 1<<SPR0; // (SPIE = 0, SPE = 1, DORD = 0, MSTR = 1, CPOL = 0, CPHA = 0, SPR1 = 0, SPR0 = 1)
		// enable the SPI, set to Main mode 0, SCK = Fosc/16, lead with MSB
		
	// Transmit the data
	PORTB &= ~(0b00000100); //(1 << PORTB2);  // Clear the SS bit to enable Secondary

	SPDR = 'R'; //Send the data 
	while ((SPSR & 0b10000000) == 0); //while ((SPSR & (0x1<<SPIF)) == 0) {} // Check the SPIF bit and wait for it to be set => transmit complete
		
	data_in =  SPDR; // read data that has come in during transmit (full-duplex mode)
	PORTD = data_in; // Send any data received to PORTD for display

	PORTB |= 0b00000100; //PORTB |= 1 << PORTB2;  // disable Secondary (this clears SBIF flag)

	while (1) {}; // infinite loop

}
