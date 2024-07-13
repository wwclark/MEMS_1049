/*
 * PWM_example.asm
 *
 *  Created: 2/27/2015 5:08:21 PM
 *   Author: WWClark
 */ 
 /*
 
  This program is an example of creating a waveform (PWM signal) using the Timer0 counter.
   
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
 
; Make sure Timer/Counter 0 is stopped, and reset it to zero.
SBI DDRD, 6 ; Make OC0A (PD6) an output bit
LDI R16, $00 ; Load $00 into OCR0 to set initial duty cycle to 0 (motor off)
OUT OCR0A, R16 
LDI R16, 0b10000011 ; Set non-inverting mode on OC0A pin (COMA1:0 and COMB0:1 		bits = bits 7:4 = 1000
		; Fast PWM (WGM1:0 bits = bits 1:0 = 11)
OUT TCCR0A, R16
LDI R16, 0b00000011 ; Set base PWM frequency (CS02:0 - bits 2-0 = 011 for prescaler of 64, for approximately 1kHz base frequency)
OUT TCCR0B, R16  ; PWM is now running on selected pin at selected duty cycle
 ; ============================================
; P R O G R A M L O O P
; ============================================
Loop_main:
 
LDI R16, 50 ; Load a number between 0-255 into OCR0A to set the duty cycle (0 = motor off, 255 = motor full on)
OUT OCR0A, R16 
 
 
	
RJMP Loop_main  ; go back to run again

 

