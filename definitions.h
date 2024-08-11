
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define INITIAL_IC_VALUE 100
#define MAX_LINE_SIZE 80
#define MAX_OPERANDS 2
#define MAX_LABEL_SIZE 31
#define NON_VALID_INTEGER 32800 /* A reserved non-valid integer that can't never be expected because it's a 15+ bit number */

/* Limits for numbers in the assembly code */
#define SIGNED_15_MIN -16384
#define UNSIGNED_15_MIN 0
#define UNSIGNED_15_MAX 32767

#define SIGNED_12_MIN -2048
#define UNSIGNED_12_MIN 0
#define UNSIGNED_12_MAX 4095

/* A note: As page 58 says in the booklet we are allowed to limit the assembler to a certain size.
           The system only has 4096 cells of memory so we limit ourselves to that size.
*/

#define ASSEMBLER_MAX_CAPACITY (4096 - INITIAL_IC_VALUE)

/* Each machine word, whether it's a code or a data word, is always made by a 15 bit integer. */
typedef struct {
    int data: 15;
} machine_word; 

/* Each operation is defined by the number of operands it needs. The group = the EXACT num of operands. 
    Groups are also in pages 35-37 of the booklet */

typedef enum {
    NO_OPERANDS = 0,
    SINGLE_OPERAND,
    TWO_OPERANDS 
} OperationGroup; 

/* Notice that we would define any unknown type of operand with address mode -1. */

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

typedef enum {
    LINE_DATA, 
    LINE_CODE
} LineType;

#endif
