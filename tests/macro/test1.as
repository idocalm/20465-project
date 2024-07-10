MAIN:   add r3, LIST 
LOOP:   prn #48
        macr macro
        cmp r3, #-6
        bne END 
        endmacr 
        lea STR, r6
        macro