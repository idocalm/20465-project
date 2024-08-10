#include "strings.h"
/*
    ------- Strings operations -------
*/

/**
    * @brief Skips all non-spaces at the beginning of a given string
    * @param p - the pointer to the string
*/
void skip_non_spaces(char **pp_line) {
    while (**pp_line && !isspace(**pp_line)) {
        (*pp_line)++; 
    }
}



/**
    * @brief Skips all spaces at the beginning of a given string
    * @param p - the pointer to the string
*/
void skip_spaces(char **p)
{
    while (**p != '\0' && isspace(**p))
    {
        (*p)++;
    }
}

/**
    * @brief Checks if a given string is composed only of space
    * @param str - the string
    * @return 1 if the string is empty, 0 otherwise
*/

int is_empty(char *str)
{
    while (*str != '\0')
    {
        if (!isspace(*str))
        {
            return 0; /* not empty */
        }
        str++;
    }
    return 1; /* empty */
}

/**
    * @brief Trims all spaces from the beginning and ending of a string
    * @param p - the pointer to the string
*/

void remove_all_spaces(char *p) {
    char *p_start = p;
    char *p_end = p + strlen(p) - 1;
    size_t trimmed = 0;

    while (isspace(*p_start)) {
        p_start++;
    }

    while (isspace(*p_end) && p_end >= p_start) {
        p_end--;
    }

    trimmed = p_end - p_start + 1;

    memmove(p, p_start, trimmed);
    p[trimmed] = '\0';

}

/**
    * @brief Copies a string from the source to the destination until a space is found
    * @param dest - the destination string
    * @param src - the source string
*/

void copy_string_until_space(char *dest, const char *src)
{
    int i = 0, j = 0;
    while (!isspace(src[j]))
    {
        dest[i] = src[j];
        i++;
        j++;
    }

    dest[i] = '\0';
}

/**
    * @brief Checks if a given string is a comment
    * @param p_line - the pointer to the string
    * @return 1 if the string is a comment, 0 otherwise
*/

int is_comment(char *p_line) {
    skip_spaces(&p_line);
    return *p_line == COMMENT_PREFIX; 
}

/**
    * @brief Checks if a given string is an integer that can be represented in 12 bits
    * @param p - the pointer to the string
    * @return the integer value if the string is an integer or NON_VALID_INTEGER 
 */

int is_integer(char *p)
{

    /*
        Note that an integer in our system can include a '+' or '-' sign at the beginning.
        So we first check if the first char is + / - and take it into account
    */

    int sign = 1;
    int num = NON_VALID_INTEGER;
    int i = 0;

    if (*p == '+' || *p == '-') 
    {
        sign = *p == '+' ? 1 : -1; 
        p++;
    }

    for (i = 0; i < strlen(p); i++)
    {
        if (!isdigit(p[i]))
        {
            return NON_VALID_INTEGER;
        }
    }

    num = atoi(p);


    return num * sign;
}

/**
    * @brief Validates a given operand list (checks for double commas, commas at the beginning / end of the list)
    * @param line - the string
    * @param line_num - the line number (for error reporting)
    * @return 1 if the operand list is valid, 0 otherwise

    This is useful basically for each code line or for .data lines
*/
int validate_operand_list(char *line, int line_num, int report_error) {
    char *operand = NULL;
    char *last = NULL;

    skip_spaces(&line);
    last = line + strlen(line) - 1;

    /* Check if after skipping the spaces there's an illegal ',' */
    if (line[0] == ',') {
        if (report_error) 
            log_error("Invalid comma in line %d.\n\tThe argument list starts with a comma.\n", line_num);
        return 0;
    }

    /* Skip spaces from the end going backwards */
    while (isspace(*last)) {
        last--;
    }

    /* Check if after skipping all the arguments, the line ends with an ',' */

    if (*last == ',') {
        if (report_error)
            log_error("Invalid comma in line %d.\n\tThe argument list ends with a comma.\n", line_num);
        return 0;
    }

    /* Check for no double commas */
    while ((operand = strchr(line, ',')) != NULL) {
        operand++; 
        skip_spaces(&operand); /* Skip spaces after the comma because ,     , is also invalid */
        if (*operand == ',') {
            if (report_error)
                log_error("Invalid comma in line %d.\n\tThe argument list contains a double comma.\n", line_num);
            return 0; /* Found an error */
        }
        line = operand + 1; /* Move pas the , */
    }

    return 1;
}

/**
    * @brief Splits a given string into operands and reports for double / invalid commas
    * @param line - the string
    * @param operands - the array of operands
    * @param operands_count - the number of operands
    * @param line_num - the line number (for error reporting)
*/

int get_operands(char *line, char **operands, int *operands_count, int line_num) {
    int i = 0;
    char *operand = NULL;

    if (!validate_operand_list(line, line_num, 1)) {
        return 0;
    }

    /* split line by ',' */
    while ((operand = strtok(line, ",")) != NULL) {

        if (i == MAX_OPERANDS + 1) {
            return 0;
        }

        operands[i] = (char *) safe_malloc(strlen(operand) + 1);

        remove_all_spaces(operand);
        strcpy(operands[i], operand);

        operands[i] = (char *) safe_realloc(operands[i], strlen(operands[i]) + 1);
        i++;
        line = NULL; 
    }
    *operands_count = i;

    return 1; 
}

/**
 * @brief Converts a given integer to an octal string
 * @param data - the integer to be converted
 * @return a string representing the octal value of the integer
 */

 
char *convert_to_octal(int data) {
    char *octal = safe_malloc(OCTAL_SIZE + 1);
    int i = 0;
    int mask = 0x7;

    for (i = 0; i < OCTAL_SIZE; i++)
    {
        octal[i] = '0';
    }
    octal[OCTAL_SIZE] = '\0';

    for (i = 0; i < OCTAL_SIZE; i++)
    {
        octal[OCTAL_SIZE - i - 1] = (data & mask) + '0';
        data >>= 3;
    }


    return octal;
}
