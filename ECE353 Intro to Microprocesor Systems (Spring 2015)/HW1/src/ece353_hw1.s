; Filename:     ece353_hw1.s 
; Author:       ece353 staff 
; Description:  

    export hw1
    export TRANSACTIONS_TOTAL
    export TRANSACTIONS
    export PERSON1
    
;**********************************************
; SRAM
;**********************************************
    AREA    SRAM, READWRITE
TRANSACTIONS		SPACE	8*1000	; Allocate 8 bytes for each transaction
TRANSACTIONS_TOTAL	SPACE	12		; Allocate 12 bytes for total
    align
        
;**********************************************
; Constant Variables (FLASH) Segment
;**********************************************
    AREA    FLASH, CODE, READONLY
PERSON1     DCB     "Steven Wiener"
            DCB     0
    align

;**********************************************
; Code (FLASH) Segment
; main assembly program
;**********************************************
hw1   PROC
    
    PUSH {R0-R12, LR}
    ; DO NOT MODIFY ANTHING ABOVE THIS LINE!!!
    ; DO NOT MODIFY REGISTERS R13-R15!!!
	
	LDR R0, =(TRANSACTIONS)	; Load transactions addr to R0
	LDR R1, =(TRANSACTIONS_TOTAL)
	MOV R7, #0 ; Count
	MOV R6, #0 ; Transaction Total
    
	; Write while loop to validate bytes, if error update trans_total to calc err
	; While loop will stop when first byte of transaction is 0
WHILE_START
	LDRB	R3, [R0, R7]
	LDR		R5, =(WHILE_END)
	CMP		R3, #0	; Compare first byte of transactions to 0
	BXEQ	R5	; If it's 0, end loop
	CMP		R3, #0x2B
	BEQ		CHECK0 	; If it's '+', continue
	CMP		R3, #0x2D
	BNE		ERROR	; If it's not '-', error
	
CHECK0
	ADD		R7, R7, #1
	LDRB	R3, [R0, R7]
	CMP		R3, #0x39
	BGT		ERROR ; Compare next number to 9, if greater, error
	CMP		R3, #0x30
	BLT		ERROR ; If less than 0, error
	
	ADD		R7, R7, #1 ; Repeat process twice
	LDRB	R3, [R0, R7]
	CMP		R3, #0x39
	BGT		ERROR
	CMP		R3, #0x30
	BLT		ERROR
	
	ADD		R7, R7, #1
	LDRB	R3, [R0, R7]
	CMP		R3, #0x39
	BGT		ERROR
	CMP		R3, #0x30
	BLT		ERROR
	
	ADD		R7, R7, #1
	LDRB	R3, [R0, R7]
	CMP		R3, #0x2E	; Check for decimal
	BNE		ERROR
	
	ADD		R7, R7, #1	; Repeat twice more for after decimal
	LDRB	R3, [R0, R7]
	CMP		R3, #0x39
	BGT		ERROR
	CMP		R3, #0x30
	BLT		ERROR
	
	ADD		R7, R7, #1
	LDRB	R3, [R0, R7]
	CMP		R3, #0x39
	BGT		ERROR
	CMP		R3, #0x30
	BLT		ERROR
	
	ADD		R7, R7, #1
	LDRB	R3, [R0, R7]
	CMP		R3, #0		; Make sure null char ends
	BNE		ERROR
	
; Add/Subtract
	SUB		R7, R7, #6	 ; Reset count
	LDRB	R8, [R0, R7] ; Hundreds Place
	SUB		R8, R8, #0x30
	MOV		R3, #0x2710
	MUL		R8, R8, R3	 ; Multiply by 10000
	
	ADD		R7, R7, #1
	LDRB	R9, [R0, R7] ; Tens Place
	SUB		R9, R9, #0x30
	MOV		R3, #0x3E8
	MUL		R9, R9, R3	 ; Multiply by 1000
	
	ADD		R7, R7, #1
	LDRB	R10, [R0, R7] ; Ones
	SUB		R10, R10, #0x30
	MOV		R3, #0x64
	MUL		R10, R10, R3
	
	ADD		R7, R7, #2
	LDRB	R11, [R0, R7] ; Tenths
	SUB		R11, R11, #0x30
	;LSL		R11, R11, #4
	MOV		R3, #0xA
	MUL		R11, R11, R3
	
	ADD		R7, R7, #1
	LDRB	R12, [R0, R7] ; Hundreths
	SUB		R12, R12, #0x30
	
	ADD		R8, R8, R9		; Total all together
	ADD		R8, R8, R10
	ADD		R8, R8, R11
	ADD		R8, R8, R12
	
	SUB		R7, R7, #6		; Decrement count
	LDRB	R3, [R0, R7]
	CMP		R3, #0x2B	; Check if deposit/withdrawal
	BEQ		SUM			; If deposit, skip next line
	NEG		R8, R8 ; Negate transaction for withdrawal
				
SUM
	ADD		R6, R6, R8 	; Add transaction to total
	ADD		R7, R7, #8	; Increment counter so it checks next transaction
	B		WHILE_START	
	
ERROR
	MOV		R3, #0x4143
	STRH	R3, [R1, #0] ; Store "CA"
	MOV		R3, #0x434C
	STRH	R3, [R1, #2] ; Store "LC"
	MOV		R3, #0x4520
	STRH	R3, [R1, #4] ; Store " E"
	MOV		R3, #0x5252
	STRH	R3, [R1, #6] ; Store "RR"
	MOV		R3, #0x524F
	STRH	R3, [R1, #8] ; Store "OR"
	MOV		R3, #0
	STRB	R3, [R1, #10] ; Store 'Null'
	B	RETURN

WHILE_END
	CMP 	R6, #0
	BLT		NEGATIVE	; If total is negative, jump ahead
	MOV		R3, #0x2B
	STRB	R3, [R1], #1 ; Store '+'
	B		NEXT
		
NEGATIVE
	MOV		R3, #0x2D
	STRB	R3, [R1], #1 ; Store '-'
	NEG		R6, R6

NEXT
	MOV		R3, #0x24
	STRB	R3, [R1], #1 ; Store '$'
	
	MOV		R3, #0xF4000
	ADD		R3, R3, #0x240	; Move 0xF4240 (decimal 1000000) to R3
	UDIV	R5, R6, R3	 	; Divide transaction total by this amount
	MUL		R3, R5, R3
	SUB		R6, R6, R3		; Subtract this from total
	ADD		R5, R5, #0x30	; Add 0x30 to number to convert back to ASCII
	STRB	R5,	[R1], #1	; Store ten-thousands
	
	MOV		R3, #0x18000
	ADD		R3, R3, #0x6A0 	; Move decimal 100000 to R3
	UDIV	R5, R6, R3		; Divide transaction total by this
	MUL		R3, R5, R3
	SUB		R6, R6, R3
	ADD		R5, R5, #0x30
	STRB	R5,	[R1], #1	; Store thousands
	
	MOV		R3, #0x2C
	STRB	R3, [R1], #1 ; Store ','
	
	MOV		R3, #0x2710		; Repeat process from previous code
	UDIV	R5, R6, R3
	MUL		R3, R5, R3
	SUB		R6, R6, R3
	ADD		R5, R5, #0x30
	STRB	R5,	[R1], #1
	
	MOV		R3, #0x3E8		; Repeat process from previous code
	UDIV	R5, R6, R3
	MUL		R3, R5, R3
	SUB		R6, R6, R3
	ADD		R5, R5, #0x30
	STRB	R5,	[R1], #1
	
	MOV		R3, #0x64		; Repeat process from previous code
	UDIV	R5, R6, R3
	MUL		R3, R5, R3
	SUB		R6, R6, R3
	ADD		R5, R5, #0x30
	STRB	R5,	[R1], #1
	
	MOV		R3, #0x2E
	STRB	R3, [R1], #1 ; Store '.'
	
	MOV		R3, #0xA		; Repeat process from previous code
	UDIV	R5, R6, R3
	MUL		R3, R5, R3
	SUB		R6, R6, R3
	ADD		R5, R5, #0x30
	STRB	R5,	[R1], #1
	
	ADD		R5, R6, #0x30	; Convert to ASCII and store
	STRB	R5,	[R1], #1
	
	MOV		R3, #0
	STRB	R3, [R1], #1 	; Store NULL
	
    ; DO NOT MODIFY ANTHING BELOW THIS LINE!!!        
RETURN
    POP {R0-R12, PC}
    
    ENDP
    align
        

    END
        
        
