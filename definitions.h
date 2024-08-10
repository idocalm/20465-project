
#ifndef globals_H
#define globals_H

#define INITIAL_IC_VALUE 100
#define MAX_LINE_SIZE 80
#define MAX_OPERANDS 2
#define MAX_LABEL_SIZE 31
#define NON_VALID_INTEGER 2048 /* The maximum number represented in 12 bits */
#define MIN_12_BIT_NUMBER -2048
#define MAX_12_BIT_NUMBER 2047


/* A note: As page 58 says in the booklet we are allowed to limit the assembler to a certain size.
           The system only has 4096 cells of memory so we limit ourselves to that size.
*/      
#define ASSEMBLER_MAX_CAPACITY (4096 - INITIAL_IC_VALUE)

/* Each machine word, whether it's a code or a data word, is always made by a 15 bit integer. */
typedef struct {
    unsigned int data: 15;
} machine_word; 

typedef enum {
    NO_OPERANDS = 0,
    SINGLE_OPERAND,
    TWO_OPERANDS 
} OperationGroup; 

typedef enum {
    UNKNOWN_ADDRESS = -1,
    IMMEDIATE = 0,
    DIRECT = 1, 
    POINTER = 2, /* a "pointer" to a register */
    REGISTER = 3 /* Operand is a register */ 
} AddressMode;


typedef enum {
    UNKNOWN_OPERATION = -1,
    MOV = 0,
    CMP,
    ADD,
    SUB,
    LEA,
    CLR,
    NOT,
    INC,
    DEC,
    JMP,
    BNE,
    RED,
    PRN,
    JSR,
    RTS,
    STOP
} Operation; /* Note that this matches the order in pages 47 or 32 of the booklet */

typedef enum {
    UNKNOWN_INSTRUCTION = -1,
    DATA = 0, /* .data */
    STRING, /* .string */
    ENTRY, /* .entry */
    EXTERN /* .extern */
} Instruction; 

typedef enum {
    UNKNOWN_REGISTER = -1,
    R0 = 0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7
} Register; 


typedef enum {
    NO_PASS_ERROR,
    FOUND_ERROR
} PassError; /* Used mainly to signal if the first / second pass wen't ok or not */

#endif
