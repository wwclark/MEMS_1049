/*
 * simple.asm
 *
 *  Created: 2/25/2015 11:11:34 AM
 *   Author: WW Clark
 */ 
 /* 
	Simple starter code for learning Studio and Assembly language.
 
	*/
; ============================================
; H E A D E R   F I L E S
; ============================================
; Included header file for target AVR type
 .NOLIST                   ;Assembler directive to not list the following in the list file
 .INCLUDE "m328Pdef.inc"   ;ATmega328P
 .LIST                     ;Switch list on again
/*
 * simple.asm
 *
 *  Created: 2/25/2015 11:11:34 AM
 *   Author: WW Clark
 */ 
 /* 
	Simple starter code for learning Studio and Assembly language.
 
	*/
; ============================================
; H E A D E R   F I L E S
; ============================================
; Included header file for target AVR type
 .NOLIST                   ;Assembler directive to not list the following in the list file
 .INCLUDE "m328Pdef.inc"   ;ATmega328P
 .LIST                     ;Switch list on again
; ============================================
; R E S E T   A N D   I N T   V E C T O R S
; ============================================
.CSEG          ;Code segment
.ORG 0000      ;Set program origin
RJMP Main      ; Reset Vector:  When the microcontroller is reset (or powered up), it sets Program Counter
				; to $0000.  The instruction at this point should jump to main loop of the program.  Will be important
				; when other interrupt vectors are used.
; ============================================
; M A I N P R O G R A M   I N I T
; ============================================
Main:

LDI R17, 0b11110000  ; define Port D bits 0-3 as input, and bits 4-7 as output
OUT DDRD, R17
; ============================================
; P R O G R A M L O O P
; ============================================
Loop_main:

NOP  ; Do nothing
RJMP Loop_main

; ============================================
; S U B R O U T I N E S
; ============================================
