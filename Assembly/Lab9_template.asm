/*
 * Lab9_template.asm
 *
 *  Created: 3/26/2021
 *   Author: wclark
 */ 
;=============================================
; INCLUDE FILES
; header file for target AVR type
 .NOLIST                   ;Don't list the following in the list file
 .INCLUDE "m328pdef.inc"   ;ATmega328p
 .LIST                     ;Switch list on again
;
; ============================================
; CONSTANTS
; [Add all constants here that can be subject
; to change by the user]
; Format: .EQU const = $ABCD
; ============================================
; SRAM PLACEMENT
.DSEG       ;Data segment
.ORG 0X100  ;Set data segment origin
; Format: Label: .BYTE N ; reserve N Bytes from Label:
; ============================================
; RESET AND INTERRUPT VECTORS
.CSEG          ;Code segment
.ORG 0x0000      ;Set program origin
	JMP Main  ; Reset vector, jump to main loop, 1.  Note that actual code is placed further down the program memory so as not to sit on top of vectors.
	; JMP Int0_name ; INT0, 2 -- Interrupt Vector for INT0.  Points to INT0 code.
	; JMP Int1_name   ;INT1, 3 -- Interrupt Vector for INT1.  Points to INT1 code.
    ;PCINT0 not used,4
	;PCINT1 not used,5
	;PCINT2 not used,6
	;WDT not used,7
	;TIMER2 COMPA not used,8
	;TIMER2 COMPB not used,9
	;TIMER2 OVF not used,10
	;TIMER1 CAPT not used,11
	;TIMER1 COMPA not used,12
	;TIMER1 COMPB not used,13
	;TIMER1 OVF not used,14
	;TIMER0 COMPA not used,15
	;TIMER0 COMPB not used,16
	;TIMER0 OVF not used,17
	;SPI,STC not used,18
	;USART,RX not used,19
	;USART,UDRE not used,20
	;USART,TX not used,21
	;ADC not used,22
	;EE READY not used,23
	;ANALOG COMP not used,24
	;TWI not used,25
	;SPM READY not used,26*/

; ============================================
; MAIN PROGRAM -- SETUP
Main:
	
