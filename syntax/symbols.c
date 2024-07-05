#include <stdlib.h>

#include <stddef.h>
#include <string.h>


#include "symbols.h"
#include "../structs/hash_table.h"
#include "../syntax/helpers.h"

ht_t *operations, *directives, *registers;

char *itoa(int num)
{
    int length = snprintf(NULL, 0, "%d", num);
    char *str = (char *) safe_malloc(length + 1);
    snprintf(str, length + 1, "%d", num);
    return str;
}

void symbols_init()
{
    
    operations = ht_create();
    directives = ht_create();
    registers = ht_create();

    ht_set(operations, "mov", itoa(OPCODE_MOV));
    ht_set(operations, "cmp", itoa(OPCODE_CMP));
    ht_set(operations, "add", itoa(OPCODE_ADD));
    ht_set(operations, "sub", itoa(OPCODE_SUB));
    ht_set(operations, "not", itoa(OPCODE_NOT));
    ht_set(operations, "clr", itoa(OPCODE_CLR));
    ht_set(operations, "lea", itoa(OPCODE_LEA));
    ht_set(operations, "inc", itoa(OPCODE_INC));
    ht_set(operations, "dec", itoa(OPCODE_DEC));
    ht_set(operations, "jmp", itoa(OPCODE_JMP));
    ht_set(operations, "bne", itoa(OPCODE_BNE));
    ht_set(operations, "red", itoa(OPCODE_RED));
    ht_set(operations, "prn", itoa(OPCODE_PRN));
    ht_set(operations, "jsr", itoa(OPCODE_JSR));
    ht_set(operations, "rts", itoa(OPCODE_RTS));
    ht_set(operations, "stop", itoa(OPCODE_STOP));

    ht_set(directives, ".data", itoa(DIRECTIVE_DATA));
    ht_set(directives, ".string", itoa(DIRECTIVE_STRING));
    ht_set(directives, ".entry", itoa(DIRECTIVE_ENTRY));
    ht_set(directives, ".extern", itoa(DIRECTIVE_EXTERN));

    ht_set(registers, "r0", itoa(REGISTER));
    ht_set(registers, "r1", itoa(REGISTER));
    ht_set(registers, "r2", itoa(REGISTER));
    ht_set(registers, "r3", itoa(REGISTER));
    ht_set(registers, "r4", itoa(REGISTER));
    ht_set(registers, "r5", itoa(REGISTER));
    ht_set(registers, "r6", itoa(REGISTER));
    ht_set(registers, "r7", itoa(REGISTER));
    
}

int is_operation(char *str)
{
    return ht_get(operations, str) != NULL;
}

int is_directive(char *str)
{
    return ht_get(directives, str) != NULL;
}

int is_register(char *str)
{
    return ht_get(registers, str) != NULL;
}

void symbols_free()
{
    ht_free(operations);
    ht_free(directives);
    ht_free(registers);
}