
#ifndef globals_H
#define globals_H


#define MAX_LINE_SIZE 80
#define MAX_OPERANDS 2
#define MAX_LABEL_SIZE 31

typedef struct {
    unsigned int ARE: 3;
    unsigned int dest: 4;
    unsigned int source: 4;
    unsigned int opcode: 4;
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

} AdressMode;

typedef enum {
    UNKNOWN_OPERATION = -1,
    MOV = 0,
    CMP,
    ADD,
    SUB,
    NOT,
    CLR,
    LEA,
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



#endif
