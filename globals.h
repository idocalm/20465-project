
#ifndef globals_H
#define globals_H

#define INITIAL_IC_VALUE 100
#define MAX_LINE_SIZE 80
#define MAX_OPERANDS 2
#define MAX_LABEL_SIZE 31
#define NON_VALID_INTEGER 2048 /* Can't be represented in 12 bits */

/* A note: We chose to limit our capacity to 1000 lines of machine words.
    Which means our assembler can at worst case support 333 lines of assembly. 
*/      
#define ASSEMBLER_MAX_CAPACITY 1000 


typedef struct {
    int ic;
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
    RELATIVE = 2, /* Operand is a "pointer" */
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
} Operation;

typedef enum {
    UNKNOWN_DIRECTIVE = -1,
    DATA = 0,
    STRING,
    ENTRY,
    EXTERN
} Directive;

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

typedef struct {
    char *name;
    int is_internal;
    int is_external;
} Label;

/* TODO: Decide if you want to continue with this approach. If so, add the errors here and work with them. If not just return an int. */
typedef enum {
    NO_PASS_ERROR,
    FOUND_ERROR
} PassError;

#endif
