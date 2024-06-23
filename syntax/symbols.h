#ifndef SYMBOLS_H
#define SYMBOLS_H

enum ESYMBOLS {
    UNKNOWN = -1,
    OPCODE_MOV,
    OPCODE_CMP,
    OPCODE_ADD,
    OPCODE_SUB,
    OPCODE_LEA,
    OPCODE_CLR,
    OPCODE_NOT,
    OPCODE_INC,
    OPCODE_DEC,
    OPCODE_JMP,
    OPCODE_BNE,
    OPCODE_RED,
    OPCODE_PRN,
    OPCODE_JSR,
    OPCODE_RTS,
    OPCODE_STOP,
    DIRECTIVE_DATA,
    DIRECTIVE_STRING,
    DIRECTIVE_ENTRY,
    DIRECTIVE_EXTERN,
    REGISTER
};

int is_operation(char *str);
int is_directive(char *str);
int is_register(char *str);

void symbols_free();

#endif