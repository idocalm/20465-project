; Macro errors here - DETECTED
macr 
    cmp r3, #-6
    bne END 
endmacr 

endmacrendmacrendmacrendmacrendmacrendmacrendmacrendmacrendmacr

macr r1 
    cmp r3, #-6
    bne END
endmacr 

macr r2
    stop 
endmacr d  

a macr test 
    cmp r3, #-6
    bne END
endmacr

.extern m_macr