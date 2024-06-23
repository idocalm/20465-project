#include "symbols.h"
#include "./structs/hash_table.h"

HashTable *operations, *directives, *registers;

void symbols_init() {
    operations = hashtable_init();
    hashtable_putint(operations, "mov", OPCODE_MOV);
    hashtable_putint(operations, "cmp", OPCODE_CMP);
    hashtable_putint(operations, "add", OPCODE_ADD);
    hashtable_putint(operations, "sub", OPCODE_SUB);
    hashtable_putint(operations, "not", OPCODE_NOT);
    hashtable_putint(operations, "clr", OPCODE_CLR);
    hashtable_putint(operations, "lea", OPCODE_LEA);
    hashtable_putint(operations, "inc", OPCODE_INC);
    hashtable_putint(operations, "dec", OPCODE_DEC);
    hashtable_putint(operations, "jmp", OPCODE_JMP);
    hashtable_putint(operations, "bne", OPCODE_BNE);
    hashtable_putint(operations, "red", OPCODE_RED);
    hashtable_putint(operations, "prn", OPCODE_PRN);
    hashtable_putint(operations, "jsr", OPCODE_JSR);
    hashtable_putint(operations, "rts", OPCODE_RTS);
    hashtable_putint(operations, "stop", OPCODE_STOP);

    directives = hashtable_init();
    hashtable_putint(directives, ".data", DIRECTIVE_DATA);
    hashtable_putint(directives, ".string", DIRECTIVE_STRING);
    hashtable_putint(directives, ".entry", DIRECTIVE_ENTRY);
    hashtable_putint(directives, ".extern", DIRECTIVE_EXTERN);

    registers = hashtable_init();
    hashtable_putint(registers, "r0", REGISTER_R0);
    hashtable_putint(registers, "r1", REGISTER_R1);
    hashtable_putint(registers, "r2", REGISTER_R2);
    hashtable_putint(registers, "r3", REGISTER_R3);
    hashtable_putint(registers, "r4", REGISTER_R4);
    hashtable_putint(registers, "r5", REGISTER_R5);
    hashtable_putint(registers, "r6", REGISTER_R6);
    hashtable_putint(registers, "r7", REGISTER_R7);
}

int is_opertion(char *str) {
    return hashtable_getint(operations, str) != -1;
}

int is_directive(char *str) {
    return hashtable_getint(directives, str) != -1;
}

int is_register(char *str) {
    return hashtable_getint(registers, str) != -1;
}

void symbols_free() {
    hashtable_free(operations);
    hashtable_free(directives);
    hashtable_free(registers);
}