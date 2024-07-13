/*
 * LCD_Example.c
 *
 * Created: 7/11/2024 3:04:44 PM
 * Author : wclar
 
 This program is a simple example of how to control a common LCD.  The program assumes a 16x2 LCD with wiring based on the Hitachi HD44780 driver using 4-bit control.
 
 Example wiring and other information can be found at: https://learn.sparkfun.com/tutorials/basic-character-lcd-hookup-guide or https://learn.adafruit.com/character-lcds/character-vs-graphical-lcds
 More information about the LCD and commands to control it can be found in the hd44780.h file (e.g. https://www.arduino.cc/reference/en/libraries/hd44780/) and data sheet, which can be found at the Sparkfun or Adafruit links.

 The HD44780 provides a parallel I/O interface.  In this example, we are using pins D4-D7 for commands/data, RS to distinguish between a command to the device (0) or data to be displayed (1), and EN
 to tell the controller to read the state of all pins and act on them when it is pulsed high. There is also a RW (read/write) pin that we'll hardwire to ground (write only).  Since we are using only D4-D7, 
 this is 4-bit mode.  This reduces the number of pins needed on the microcontroller but requires the commands/data to be divided into upper and lower nibbles when sent, as can be seen in the functions below.  
 There is also an 8-bit mode that uses pins D0-D7. 
 
 When operating in 4-bit mode, the process to issue a command is:
	Set RS to high (for displaying the data being sent by the D pins) or low (for sending a command byte to the chip)
	Set D4, D5, D6, D7 to the 4th, 3rd, 2nd, and high nibble of the command/data
	Pulse the EN pin by setting it low, high, then low
	Set D4, D5, D6, and D7 to the low nibble of the command/data
 	Pulse the EN pin
 Note that delays are needed after each step.  The delays in this example code are don't represent specific limits of operation (e.g. per the data sheet, pulsing EN requires ~1 microsecond delays and 
 processing a command takes ~40microseconds).  Our wait function accepts milliseconds as the argument, so the delays are longer than needed.  Changing the delay in the LCD_data() function can cause the characters 
 to appear in a faster or slower scroll.
 
 This example code illustrates basic use of the display.  For more extensive functionality, see the data sheets, libraries, and other tutorial information.
 
 
 */ 

#define FREQ_CLK 1000000

#include <avr/io.h>
#include <string.h>

#define lcd_port PORTD // We are using port D for the LCD pins (0 = register select, 1 = enable, 4-7 = data)
#define lcd_EN 1       // enable pin on LCD
#define lcd_RS 0       // register select on LCD

void wait(volatile unsigned int);
void LCD_write(unsigned char cmd);
void LCD_init(void);
void LCD_command(char);
void LCD_data(char);
void LCD_print(char *str);
void LCD_gotoxy(unsigned char x, unsigned char y);

int main() {
	DDRD = 0xF3; // Set PD0, PD1, and PD4-7 to output for the LCD
	LCD_init();  // initialize the LCD
	wait(50);

	LCD_gotoxy(1, 1);         // Go to the location 1,1 of lcd // or can use LCD_command(0x80); to move to location 1,2
	LCD_print("  Mechatronics"); // Print the text
	LCD_gotoxy(1, 2);         // Go to the location 1,2 of lcd 	// or can use LCD_command(0xA9); to move to location 1,2
	LCD_print("     Rocks!");  // Print the text
	
	while (1) {};

	return 0;
}

void LCD_init(void) { // initialize the LCD
	LCD_command(0x02); // set the LCD to 4-bit control mode
	wait(1);
	LCD_command(0x28); // set the LCD to 2 lines, 5X7 dots, 4-bit mode
	wait(1);
	LCD_command(0x01); // clear LCD
	wait(1);
	LCD_command(0x0E); // turn LCD cursor ON
	wait(1);
	LCD_command(0x80); //  set cursor at first line, first position (positions are defined by lower 7 bits and are 1-40 for line 1 and 41-80 for line 2, though
							// positions 17-40 and 57-80 are not visible on a 16-column display
	wait(1);
	return;
} // end LCD_init()

void LCD_gotoxy(unsigned char x, unsigned char y) {  // move the LCD cursor to a given position on the LCD screen
	unsigned char firstCharAdr[] = {0x80, 0xC0, 0x94, 0xD4};
	LCD_command(firstCharAdr[y - 1] + x - 1);
	wait(1);
} //end LCD_gotoxy

void LCD_command(char command_value) { // send a command to the LCD driver -- 1/2 nibble at a time
	lcd_port = command_value & 0xF0;          // Send upper nibble (mask lower nibble because PD4-PD7 pins are used for data)
	lcd_port &= ~(1 << lcd_RS); // lcd_RS = 0 for command
	lcd_port |= (1 << lcd_EN);  // toggle lcd_EN bit
	wait(1);
	lcd_port &= ~(1 << lcd_EN); // toggle bit back off
	wait(10);
	lcd_port = ((command_value << 4) & 0xF0); // Send lower nibble (shift 4-bits and mask for lower nibble)
	lcd_port &= ~(1 << lcd_RS); // lcd_RS = 0 for command
	lcd_port |= (1 << lcd_EN);  // toggle lcd_EN bit
	wait(1);
	lcd_port &= ~(1 << lcd_EN); // toggle bit back off
	wait(10);
} // end LCD_command()

void LCD_data(char data_value) { // send data to the LCD driver -- 1/2 nibble at a time
	lcd_port = data_value & 0xF0; // Send upper nibble (mask lower nibble because PD4-PD7 pins are used for data)
	lcd_port |= (1 << lcd_RS);  // lcd_RS = 1 for data
	lcd_port |= (1 << lcd_EN);  // toggle lcd_EN bit
	wait(1);
	lcd_port &= ~(1 << lcd_EN); // toggle bit back off
	wait(10);
	lcd_port = ((data_value << 4) & 0xF0); // Shift 4-bit and mask
	lcd_port |= (1 << lcd_RS);  // lcd_RS = 1 for data
	lcd_port |= (1 << lcd_EN);  // toggle lcd_EN bit
	wait(1);
	lcd_port &= ~(1 << lcd_EN); // toggle bit back off
	wait(10);
} // end LCD_data()

void LCD_print( char *str) { // send string information to the LCD driver to be printed (the argument stores the address of the string in pointer *str)
	int i = 0;
	while (str[i] != '\0') {     // loop until NULL character in the string
		LCD_data(str[i]); // sending data on LCD byte by byte
		i++;
	}
	return;
} // end LCD_print()

void wait(volatile unsigned int number_of_msec) {
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