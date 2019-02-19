;
; main.asm
;
; Created: 2/13/2019 1:33:19 PM
; Author : chent
;

.ORG 0
	LDI	r16, 0b00001000	; Counter
	LDI	r24, 0b00001111	; Mulitplicand low bit
	LDI	r25, 0b00001001	; Mulitplicand High bit
	CLR	r18				; LOW
	CLR	r19				; MID
	CLR	r20				; HIGH
	LDI	r21, 0b00000000	; index counter
	LDI	r22, 0b00000101	; Multiplier
	LDI	r23, 0b00000000	; Temp variable
	
INDEX: ; Using bitmasking to find if it should go to shift or shift and add
	MOV	r17, r22			; Bit masking index
	ANDI r17, 0b00000001	; Using bit masking to check the multlipier
	BRNE SETUPVALUE			; If it is 1 than go to setup the value
	BREQ SHIFT				; If it is 0 go to shift

SHIFT:
	LSL	r23			; shift to the left of the temp variable
	LSL	r25			; shift to the left of the high value
	ADC	r23, r21	; add carry to the high value
	LSL	r24			; shift the low bit 
	ADC	r25, r21	; add carry to the high value
	LSR	r22			; shift right of the multipiler
	DEC	r16			; counter --
	BRNE INDEX		; if not 0 got back to bit masking
	BREQ Done		; if it is 0 than done

SETUPVALUE:
	ADD	r18, r24	; add the low value into final low
	ADC	r19, r21	; add the low carry into mid
	ADC	r20, r21	; add the low carry into high
	ADD	r19, r25	; add the mid value into final mid
	ADC	r20, r21	; add the mid carry into high
	ADC	r20, r23	; add the temp variable into the high value
	RJMP	SHIFT 

Done:
	
	BREAK 


