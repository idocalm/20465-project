add r3, LIST 
prn #48
macr m_macr 
cmp r3, #-6
bne END 
endmacr 
lea STR, r6
inc r6 
mov *r6,K 
sub r1, r4 
m_macr
dec K 
jmp LOOP 
stop 
.string “abcd” 
.data 6, -9 
.data -100
.data 31