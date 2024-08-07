#include "symbols.h"

/**
    * Finds the matching operation to a string (if such exists).
    * @param str - the string. 
    * @return the operation that matches the string.
*/

Operation get_operation(char *str)
{
    int i;
    operation_search operations[] = {
        {"mov", MOV},
        {"cmp", CMP},
        {"add", ADD},
        {"sub", SUB},
        {"not", NOT},
        {"clr", CLR},
        {"lea", LEA},
        {"inc", INC},
        {"dec", DEC},
        {"jmp", JMP},
        {"bne", BNE},
        {"red", RED},
        {"prn", PRN},
        {"jsr", JSR},
        {"rts", RTS},
        {"stop", STOP}
    };
    int length = sizeof(operations) / sizeof(operation_search);

    for (i = 0; i < length; i++)
    {
        /* Check if the string is equal to the directive name */
        if (strcmp(str, operations[i].name) == 0)
        {
            return operations[i].operation;
        }
    }

    /* Return not found */
    return UNKNOWN_OPERATION;
}

/**
    * Finds the directive command that matches a string (if such exists).
    * @param str - the string. 
    * @return the directive that matches the string.
*/

Directive get_directive(char *str)
{
    int i; 
    directive_search directives[] = {
        {".data", DATA},
        {".string", STRING},
        {".entry", ENTRY},
        {".extern", EXTERN}
    };

    int length = sizeof(directives) / sizeof(directive_search);
    for (i = 0; i < length; i++)
    {
        /* Check if the string is equal to the directive name */
        if (strcmp(str, directives[i].name) == 0)
        {
            return directives[i].directive;
        }
    }

    /* Return not found */
    return UNKNOWN_DIRECTIVE;
}

/**
    * Finds responding register to a string (if exists).
    * @param str - the string
    * @return the register that corresponds to the string.
*/

Register get_register(char *str)
{
    /* All registers are exactly 2 letters */
    if (strlen(str) != 2) 
    {
        return UNKNOWN_REGISTER;
    }

    /* Check that the string is 2 characters long, with 'r' as the first character and then a digit */
    if (str[0] == 'r' && isdigit(str[1]))
    {
        int digit = str[1] - '0'; /* Convert the digit to an integer using ASCII */
        return digit >= 0 && digit <= 7 ? digit : UNKNOWN_REGISTER;
 
    }
    /* Return not found */
    return UNKNOWN_REGISTER;
}


/**
    * Checks if a string is a reserved word. 
    * @param str - the string
    * @return 1 if the string is a reserved word, 0 otherwise.
*/

int is_reserved_word(char *str)
{
    /* Reserved words are any operations, registers or directives */
    return get_operation(str) != UNKNOWN_OPERATION || get_register(str) != UNKNOWN_REGISTER || get_directive(str) != UNKNOWN_DIRECTIVE;
}

/**
    * Checks if the beginning of a line has an error related to a label definition.
    * @param line - the line.
    * @param line_num - the line number (That's used to report any errors).
    * @param dest - a string that was malloced to store the label.
    * @param report_error - indicates if the function should report an error if it sees one.
 */

int is_label_error(char *line, int line_num, char *dest, int report_error) {
    int j = 0;
    char *ptr;

    skip_spaces(&line);
    ptr = line;

    for (; *ptr && *ptr != ':' && *ptr != '\n'; j++, ptr++) {
        if (j > MAX_LINE_SIZE) {
            if (report_error) {
                log_error("Invalid label in line %d\n\tLabel: %s is too long\n", line_num, dest);
            }
            dest[0] = '\0'; /* Signal it's not a definition */

            return 1; /* There's an error */
        }
        dest[j] = *ptr;
    } 
    dest[j] = '\0'; /* Null terminatingg */

    if (*ptr == ':') {
        if (!is_label(dest)) { /* Use is_label to check if the label itself is ok */
            if (report_error) {
                log_error("Invalid label in line %d\n\tLabel: %s is not a valid label\n", line_num, dest);
            }
            dest[0] = '\0'; /* Signal it's not a definition */
            return 1; /* There's an error */
        } 
        return 0; /* No error */
    }
    dest[0] = '\0'; /* Signal it's not a definition */

    return 0; /* No error */
}

/**
    * Checks if a string could be used as a label.
    * @param label - the string.
    * @return 1 if the label is valid, 0 otherwise.
 */
int is_label(char *label) {
    char *ptr = label;
    
    /* TODO FIX THIS */
    int isMacro = 0;

    /* Label must contain only letters and digits */
    while (*ptr != '\0' && *ptr != '\n') {
        if (!isalpha(*ptr) && !isdigit(*ptr)) {
            return 0;
        }
        ptr++;
    }

    /* A label can't either be a reserved word or be more then 31 chars long */
    return is_reserved_word(label) == 0 && strlen(label) <= MAX_LABEL_SIZE && !isMacro;
}


/**
    * Checks if a command is valid with a set of given addressing modes as operands.
    * @param op - the operation to check.
    * @param dest - the destination addressing mode. (if exists)
    * @param source - the source addressing mode. (if exists)
 */

int valid_command_with_operands(Operation op, AddressMode dest, AddressMode source) {
    
    Operation no_source_ops[] = {CLR, NOT, INC, DEC, JMP, BNE, RED, PRN, JSR};
    int i; 

    /* The table uses information from page 47 of the booklet */

    valid_command_modes valid_table[] = {
        {MOV, {1, 1, 1, 1}, {-1, 1, 1, 1}},
        {CMP, {1, 1, 1, 1}, {1, 1, 1, 1}},
        {ADD, {1, 1, 1, 1}, {-1, 1, 1, 1}},
        {SUB, {1, 1, 1, 1}, {-1, 1, 1, 1}},
        {LEA, {-1, 1, -1, -1}, {-1, 1, 1, 1}},
        {CLR, {-1, -1, -1, -1}, {-1, 1, 1, 1}},
        {NOT, {-1, -1, -1, -1}, {-1, 1, 1, 1}},
        {INC, {-1, -1, -1, -1}, {-1, 1, 1, 1}},
        {DEC, {-1, -1, -1, -1}, {-1, 1, 1, 1}},
        {JMP, {-1, -1, -1, -1}, {-1, 1, 1, 1}},
        {BNE, {-1, -1, -1, -1}, {-1, 1, 1, -1}},
        {RED, {-1, -1, -1, -1}, {-1, 1, 1, -1}},
        {PRN, {-1, -1, -1, -1}, {1, 1, 1, 1}},
        {JSR, {-1, -1, -1, -1}, {-1, 1, 1, -1}},
    };
    size_t no_source_ops_size = sizeof(no_source_ops) / sizeof(Operation);

    if (op == RTS || op == STOP)
    {
        /* RTS and STOP have no operands */
        return dest == UNKNOWN_ADDRESS && source == UNKNOWN_ADDRESS;
    }

    if (dest == UNKNOWN_ADDRESS) {
        /* There are no operations that have no dest operand except RTS and STOP */
        return 0;
    } 

    if (source == UNKNOWN_ADDRESS) {
        int flag = 0;
        /* Check if the op is supposed to have no source operand */
        for (i = 0; i < no_source_ops_size; i++) {
            if (op == no_source_ops[i]) {
                flag = 1;
                break;
            }
        }

        if (!flag) {
            return 0;
        }

        /* The table has 1 on addressuing modes that are valid */
        return valid_table[op].dest_modes[dest] != -1;
    }

    return valid_table[op].dest_modes[dest] != -1 && valid_table[op].source_modes[source] != -1;

}


/** 
    * Returns the operation group of an operation. (operands count)
    * @param op - the operation.
    * @return the operation group.
 */

OperationGroup get_operation_group(Operation op) {
    if (op == MOV || op == CMP || op == ADD || op == SUB || op == LEA) {
        return TWO_OPERANDS;
    } else if (op == CLR || op == NOT || op == INC || op == DEC || op == JMP ||
         op == BNE || op == RED || op == PRN || op == JSR 
    ) {
        return SINGLE_OPERAND;
    } else {
        return NO_OPERANDS;
    }
}


/**
    * Finds the addressing mode of an operand.
    * @param operand - the operand.
    * @return the addressing mode.
 */

AddressMode address_mode(char *operand) {
    if (operand[0] == '#' && is_integer(operand + 1) != NON_VALID_INTEGER) { /* Immediate addressing has '#' followed by a number */
        return IMMEDIATE; 
     } else if (get_register(operand) != UNKNOWN_REGISTER) { /* 'Register addressing' has 'r' followed by a number between 0-7 */
        return REGISTER;
    } else if (operand[0] == '*' && get_register(operand + 1) != UNKNOWN_REGISTER) { /* 'Relative addressing' has '*' followed by a register */
        return RELATIVE;
    } else if (is_label(operand)) { /* 'Direct addressing' is a label */
        return DIRECT;
    }

    return UNKNOWN_ADDRESS;
}