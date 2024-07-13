/*
 * interrupt_example.asm
 *
 *  Created: 3/31/2014 8:38:09 AM
 *   Author: wclark
 */ 
 ; ********************************************
; * H E A D E R *
; ********************************************
;
; Included header file for target AVR type
 .NOLIST                   ;Don't list the following in the list file
 .INCLUDE "m328pdef.inc"   ;ATmega328p
 .LIST                     ;Switch list on again
;
; ============================================
; C O N S T A N T S  T O  C H A N G E
; ============================================
; [Add all constants here that can be subject
; to change by the user]
; Format: .EQU const = $ABCD

 ; ============================================
; R E G I S T E R  D E F I N I T I O N S
; ============================================
;
; [Add all register names here, include info on
; all used registers without specific names]
; Format: .DEF rmp = R16

;
; ============================================
; S R A M  D E F I N I T I O N S
; ============================================

.DSEG       ;Data segment
.ORG 0X100  ;Set data segment origin
; Format: Label: .BYTE N ; reserve N Bytes from Label:

; ============================================
; R E S E T  A N D  I N T  V E C T O R S
; ============================================
.CSEG          ;Code segment
.ORG 0x0000      ;Set program origin
	JMP Main  ; Reset vector, jump to main loop, 1.  Note that actual code is placed further down the program memory so as not to sit on top of vectors.
	JMP Int0_name ; INT0, 2 -- Interrupt Vector for INT0.  Points to INT0 code.
	JMP Int1_name   ;INT1, 3 -- Interrupt Vector for INT1.  Points to INT1 code.
/*	RETI ;PCINT0 not used,4
	RETI ;PCINT1 not used,5
	RETI ;PCINT2 not used,6
	RETI ;WDT not used,7
	RETI ;TIMER2 COMPA not used,8
	RETI ;TIMER2 COMPB not used,9
	RETI ;TIMER2 OVF not used,10
	RETI ;TIMER1 CAPT not used,11
	RETI ;TIMER1 COMPA not used,12
	RETI ;TIMER1 COMPB not used,13
	RETI ;TIMER1 OVF not used,14
	RETI ;TIMER0 COMPA not used,15
	RETI ;TIMER0 COMPB not used,16
	RETI ;TIMER0 OVF not used,17
	RETI ;SPI,STC not used,18
	RETI ;USART,RX not used,19
	RETI ;USART,UDRE not used,20
	RETI ;USART,TX not used,21
	RETI ;ADC not used,22
	RETI ;EE READY not used,23
	RETI ;ANALOG COMP not used,24
	RETI ;TWI not used,25
	RETI ;SPM READY not used,26*/
; ============================================
; I N T E R R U P T  S E R V I C E S
; ============================================
;
Int0_name:
	PUSH R16 // put R16 data onto the stack in case there is useful information in that register from Main program
	IN R16, SREG 
	PUSH R16 // push Status Register onto the Stack

	// May have to preserve other important register information. Note that the program counter is automatically pushed to the stack.

	// Do some interrupt stuff.

	// Clean up before exiting ISR:
	LDI R16, 0b00000001
	OUT EIFR, R16 ; clear the INT0 flag bit (in case the interrupt was set during the ISR -- as in switch bounce). Note that this 
	; will not affect the INT1 flag, in case it was triggered during this ISR.
	POP R16
	OUT SREG, R16 // pull Status Register off the Stack (make sure that there is nothing else on top of it on the Stack
				// at this point so that proper SREG info is pulled)
	POP R16 // return original data to R16

	RETI // Return from interrupt.  Program Counter is reloaded with instruction address in main code where it left off.

Int1_name:
	PUSH R16 // put R16 data onto the stack in case there is useful information in that register from Main program
	IN R16, SREG
	PUSH R16 // push Status Register onto the Stack

	// Do some more interrupt stuff.

	LDI R16, 0b00000010 ; clear the INT1 flag bit (in case the interrupt was set during the ISR -- as in switch bounce). 
	OUT EIFR, R16
	POP R16
	OUT SREG, R16 // pull Status Register off the Stack (make sure that there is nothing else on top of it on the Stack
				// at this point so that proper SREG info is pulled)
	POP R16 // return original data to R16

	RETI
;
; ============================================
; M A I N P R O G R A M  I N I T
; ============================================
Main:
	
	/* Initialize stack */
	LDI R16, HIGH(RAMEND) ; Init HSB stack
	OUT SPH,R16
	LDI R16, LOW(RAMEND)  ; Init LSB stack
	OUT SPL,R16           ; to stack pointer

	/* Enable External Interrupts */
	/* EIMSK: external interrupt mask register */
	LDI R16, (1<<INT0)|(1<<INT1)  ;enable EXINT 0 and EXINT 1
	OUT EIMSK, R16

	/* Select type of interrupt */
	/* EICRA: external interrupt control register */
	LDI R16, (1<<ISC01)|(1<<ISC00)|(1<<ISC10)|(1<<ISC11) ;EXINT0: rising edge, EXINT1: rising edge
	/* Attention: cannot use OUT to load value into EICRA, must use STS */
	STS EICRA, R16
	 
	/*********/
	; Initialize ports and do other housekeeping before you main code starts.
	/*********/

	SEI // Set Global Interrupt Enable.  Interrupts can occur after this point.
;
; ============================================
; P R O G R A M L O O P
; ============================================
Loop_main: 

	NOP  
	// Do some main stuff.  The interrupt(s) will occur sometime during this code.

RJMP Loop_main






