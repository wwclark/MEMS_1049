;
; blink_using_timer_register.asm
;
; Created: 3/30/2020 11:25:20 AM
; Author : WW Clark
;
; This program blinks a pattern of LEDs on and off at a rate of 2 Hz (on for 250msec then switch to alternate pattern for 250msec). 
; The delays are created using the Timer0 register.
;
; Include header file for target AVR type
 .NOLIST                   ;Assembler directive to not list the following in the list file
 .INCLUDE "m328Pdef.inc"   ;ATmega328P
 .LIST                     ;Switch list on again

.CSEG          ;Code segment
.ORG 0000      ;Set program origin
RJMP Main      ; Reset Vector:  When the microcontroller is reset (or powered up), it sets Program Counter
				; to $0000.  The instruction at this point should jump to main loop of the program.  Will be important
				; when other interrupt vectors are used.

Main:
LDI R17, 0b11111111  ; define Port C bits output
OUT DDRC, R17
OUT PORTC, R17		; turn off all LEDs (they are active low)

Loop_main:
LDI R17, 0b10101010
OUT PORTC, R17
CALL wait
LDI R17, 0b01010101
OUT PORTC, R17
CALL wait
RJMP Loop_main

wait: // this subroutine creates a delay of 0-255 msec by repeatedly calling delay_1msec
	LDI R22, 250 // choose the number of msec to delay
	count_down:
	CALL delay_1msec
	DEC R22
	BRNE count_down
	RET // return from subroutine

delay_1msec: // this subroutine creates a 1msec delay with TIMER0 register and prescaler of 64 (assumes 16MHz clock)
	LDI R20, 0x00 // clears WGM00 and WGM01 (bits 0 and 1) to ensure Timer/Counter is in normal mode.
	OUT TCCR0A, R20
	
	LDI R20, 0x00 // clear timer register
	OUT TCNT0, R20
	
	LDI R21, 250 // set desired count value for later compare
		
	LDI R20, 0b00000011 //1<<CS01 | 1<<CS00;	TCCR0B = 0x03;  // Start TIMER0, Normal mode, crystal clock, prescaler = 64
	OUT TCCR0B, R20

	keep_counting:
	IN R20, TCNT0  //read TCNT0 and compare to desired count.  Loop until count = 250.
	CPSE R20, R21
	RJMP keep_counting

	LDI R20, 0x00; // Stop TIMER0
	OUT TCCR0B, R20
	RET // return from subroutine