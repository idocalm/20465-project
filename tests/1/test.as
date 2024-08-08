
; Error: double , in .data - DETECTED
.data 6,, -9 
; Error: extra , in .data - DETECTED
.data 6,

; Error: .string 
.string "Hello 
.string Hello"
.string 6
.string "Hello", ddd

; Error: entry has more then one parameter - DETECTED
.entry MAIN, LOOP
.entry MAIN LOOP 


; Error: LOOP defined as entry & extern - DETECTED
.entry LOOP
.extern LOOP


; Warning: Label at the beginning of a .entry / .extern line
CHECKA: .entry MAIN
CHECKB: .extern L3

; Error: label is not ended with : - DETECTED 
CHECKC .entry MAIN
CHECKD CHECKE: prn #48

; Error: label is not ended correctly - DETECTED
CHECKF : .extern L3

; Error: Illegal label name - DETECTED
r1: stop

add: stop

