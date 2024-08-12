; Macro with no name
macr 
    stop 
endmacr 


; Macro with extra 
macr name extra 
    stop
endmacr

; Valid macro 
macr valid 
    stop
endmacr 

; Already defined macro
macr valid 
    add r1, r2
endmacr

; Macro has extra after endmacr
macr valid2
    stop
endmacr extra
