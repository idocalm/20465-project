
; Line too long
A: .string "garbage" extraaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

; Missing commas 
ADD1: add r1 r2

; Following commas 
ADD2: add r1,, r2

; Extra commas
ADD3: add r1, r2,

; Following & extra commas
ADD4: add r1,, r2,

; Extra text
ADD5: add r1, r2 extra

; Too many arguments
ADD6: add r1, r2, r3

; Too few arguments
ADD7: add r1

; Duplicate label (ADD7 was already defined)
ADD7: add r1, r2

; Invalid label name - has _
ADD_8: add r1, r2

; Invalid label name - starts with Integer
9ADD9: add r1, r2

; Invalid string
STR1: .string "abcd" extra

