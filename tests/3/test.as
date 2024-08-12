
; Addition

macr addition
        mov      FIVE, r1
        mov      TEN, r2
        add      r1, r2
        prn      r2
endmacr 

macr multiplication
        mov      FIVE, r1
        mov      TEN, r2
        clr      r3
        clr      r4
        LOOP:    cmp      r1, #0
                 bne      END
                 add      r2, r3
                 dec      r1
                 jmp      LOOP
        END:     prn      r3
                 stop
endmacr

addition
multiplication

FIVE:  .data    5
TEN:   .data    10