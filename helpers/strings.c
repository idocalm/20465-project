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

    if (num < MIN_12_BIT_NUMBER || num > MAX_12_BIT_NUMBER) 
    {
        return NON_VALID_INTEGER;
    }
    

    return num * sign;
}

/**
    * @brief Splits a given string into operands
    * @param line - the string
    * @param operands - the array of operands
    * @param operands_count - the number of operands
*/

void get_operands(char *line, char **operands, int *operands_count) {
    int i = 0;
    char *operand = NULL;

    /* split line by ',' */
    while ((operand = strtok(line, ",")) != NULL) {


        if (i == MAX_OPERANDS + 1) {
            return;
        }

        operands[i] = (char *) safe_malloc(strlen(operand) + 1);

        remove_all_spaces(operand);
        strcpy(operands[i], operand);

        operands[i] = (char *) safe_realloc(operands[i], strlen(operands[i]) + 1);
        i++;
        line = NULL; 
    }
    *operands_count = i;
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
