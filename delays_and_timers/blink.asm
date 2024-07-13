// Assembly Language Example
/*
 * blink.asm
 *
 *  Created: 2/17/2015 8:38:11 PM
 *   Author: WW Clark
 */ 
/* 
 *
  This program blinks an LED at set frequencies.  Connect LED to PD0.
    Used to test timer options.
    Connect LED signal to oscilloscope to test frequencies.
	
 */ 
  
; ============================================
; H E A D E R   F I L E S
; ============================================
; Included header file for target AVR type
 .NOLIST                   ;Assembler directive to not list the following in the list file
 .INCLUDE "m328Pdef.inc"   ;ATmega328p
 .LIST                     ;Switch list on again
 ; ============================================
; C O N S T A N T S  T O  C H A N G E
; ============================================
; [Add all constants here that can be subject
; to change by the user]
; Format: .EQU const = $ABCD
 
 .equ   freqclk = 16000000 ;system clock frequency
 
; ============================================
; R E G I S T E R   D E F I N I T I O N S
; ============================================
; [Add all register names here, include info on
; all used registers without specific names]
; Format: .DEF rmp = R16
 
; ============================================
; S R A M   D E F I N I T I O N S
; ============================================
.DSEG         ;Data segment
.ORG 0X100    ;Set data segment origin
 
; ============================================
; R E S E T   A N D   I N T   V E C T O R S
; ============================================
.CSEG          ;Code segment
.ORG 0000      ;Set program origin
RJMP Main      ; Reset vector, jump to main loop, 1
 
; ============================================
; M A I N P R O G R A M   I N I T
; ============================================
Main:
 
LDI R17, 0b11111111  ; define all Port D bits as output
OUT DDRD, R17
 
; ============================================
; P R O G R A M L O O P
; ============================================
Loop_main:
 
; Send alternating signal to LED.
 
CBI PORTD, 0 ; turn off LED
 ; Wait
   CALL wait
 SBI PORTD, 0 ; turn on LED
 ; Wait
   CALL wait
 
RJMP Loop_main
 
; ============================================
; S U B R O U T I N E S
; ============================================
/* wait */
// This subroutine calls others to create a delay
wait:
LDI R17, 250 /* load a working register with number of msec to delay */
delayCount:
//CALL delayNms_timer0
CALL delay1ms
	DEC  R17
	BRNE delayCount
RET
 
 
/* delay 1 ms */
// This subroutine requires 1000 microseconds to execute with 16MHz clock, so is a 1 msec delay 
delay1ms:
	PUSH YL        ;2 cycles
	PUSH YH        ;2 cycles
	LDI YL, low (((freqclk/1000)-18)/4) ;1 cycle
	LDI YH, high(((freqclk/1000)-18)/4)  ;1 cycle		
		
	
	short_delay:
		SBIW YH:YL, 1     ;2 cycles
		BRNE short_delay  ;2 cycles
 
	POP YH     ;2 cycles
	POP YL     ;2 cycles
RET  ;4 cycles
