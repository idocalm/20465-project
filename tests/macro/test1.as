MAIN:   add r3, LIST              
LOOP:   prn #48
        macr macro
        stop
        endmacr 
        lea STR, r6
        macro
END:    stop
LIST:   .data 6, -9, 15
; This is a comment
; This is another comment