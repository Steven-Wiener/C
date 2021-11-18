    export insertion_sort
    AREA    FLASH, CODE, READONLY
    ALIGN
    
;******************************************************************************
; Description
; Uses Insertion Sort to sort an array of unsigned 32-bit numbers. Valid
; sizes are 0-1024
; Parameters
; R0 - Array Address
; R1 - Size of the array. 
;
; Returns
;   0 if parameters are valid.  -1 if any of the parameters are invalid. 
;******************************************************************************
insertion_sort             PROC
    ENDP
    END     
