    export bubble_sort
        
    AREA    FLASH, CODE, READONLY
    ALIGN
        
;******************************************************************************
; Description
;   Uses Bubble Sort to sort an array of unsigned 32-bit numbers.  Prior to 
;   returning, call the function verifyArray to verify that your array was sorted
;   correctly.

; Parameters
;   R0 - Array Address
;   R1 - Array Size
;******************************************************************************
bubble_sort PROC

    ; Save registers
     PUSH   {R4, R5, LR}
    

    ; R2 Current Byte Address
    ; R3 Innter loop index
    ; R4 num at lower index
    ; R5 num at higher index

    ; We only need N-1 Outer Loops
    SUBS     R1, R1, #1
    
outter_start
    BLE     end_bubble_sort
    
    ; Set the inner index to zero.  Loop until R3 == R1
    MOV     R3, #0

inner_start
    ; Check to see if we teminate the inner loop
    CMP     R1, R3   
    BEQ     inner_end
    
    ; Calculate address of the current unsigned 32-bit number
    ADD     R2, R0, R3, LSL #2
    
    ; See if we need to swap the values
    LDR    R4, [R2]
    LDR    R5, [R2, #4]
    CMP     R4, R5
    STRHI  R4, [R2, #4]
    STRHI  R5, [R2]
    
    ; Increment the index
    ADD     R3, R3, #1
    
    ; Branch to the start of the inner loop
    B       inner_start
inner_end

    
    ; Decrement loop count
    SUBS    R1, R1, #1
    
    ; Branch to the start of the outter loop
    B       outter_start

end_bubble_sort
    
    ; Restore Registers
    POP     {R4, R5, LR}
    
    ; Return from the function
    BX      LR
    
    ENDP
         
    END     

