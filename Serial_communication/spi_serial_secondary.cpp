/*
 * SPI_Secondary.cpp
 *
 * Created: 3/25/2018 10:23:37 AM
 * Author : Buddy
 
  This program sets up SPI in Secondary mode, mode 0, with SCK frequency of Fosc/16.
    This is the counterpart to the SPI_main.cpp code
    Then data is transmitted and received.
	
  SPI is operating in full duplex, so data can go out and in from/to Main/Secondary.
  
  
 */ 

#include <avr/io.h>


int main(void)
{
  // setup
  char data_in; // This variable will be used to hold incoming data

  DDRD = 0xFF; // Set DDRD to display data that comes in from Main
  // set up SPI I/O
  DDRB = 0b00010000; //1<<PORTB4;  // Set pin MISO as output
  
  // Set up Secondary SPI
  SPCR = 0b01000001; //SPCR = 1<<SPE | 1<<SPR0; // (SPIE = 0, SPE = 1, DORD = 0, MSTR = 0, CPOL = 0, CPHA = 0, SPR1 = 0, SPR0 = 1)
  // enable the SPI, set to Secondary mode 0, SCK = Fosc/16, lead with MSB
  
  //Transmit_if_needed:
  SPDR = 'N';  // Move data to be transmitted to SPDR.  In this example, the letter N (Note this is interpreted as the ASCII code: 'N' = 0x4E)
	// Data transfer from Secondary does not begin until it detects clock from main

  while ((SPSR & 0b10000000) == 0); //while (SPSR & (0x1<<SPIF)) == 0) {} // Check the SPIF bit and wait for it to be set => transmit complete

  data_in = SPDR;  // read data that has come in 
  PORTD = data_in;  // display data on PORTD
    
  while (1) { } // infinite loop
}