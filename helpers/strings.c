#include "strings.h"
/*
    ------- Strings operations -------
*/

/**
    * @brief Skips all spaces at the beginning of a given string
    * @param p - the pointer to the string
*/
void skip_spaces(char **p)
{
    /* Move the pointer one step until it's not a space anymore */
    while (**p != '\0' && **p != '\n' && isspace(**p))
    {
        (*p)++;
    }
}

/**
    * @brief Trims all spaces from the beginning and ending of a string
    * @param p - the pointer to the string
*/

void remove_all_spaces(char *p) {

    char *p_start = p; /* One pointer to the start */
    char *p_end = p + strlen(p) - 1; /* One pointer to the end */
    size_t trimmed = 0;

    while (isspace(*p_start)) { /* Skip all spaces from the beginning */
        p_start++;
    }

    while (isspace(*p_end) && p_end >= p_start) { /* Skip all spaces from the end */
        p_end--;
    }

    trimmed = p_end - p_start + 1; /* New size */

    memmove(p, p_start, trimmed); /* Move the string to the beginning */
    p[trimmed] = '\0'; /* Null  term */

}

/**
    * @brief Copies a string from the source to the destination until a space is found
    * @param src - the source string
*/

char* copy_string_until_space(char *src)
{
    char *output = (char *)malloc(MAX_LINE_SIZE + 1); /* Allocate for the new string */
    int i = 0, j = 0;
    while (!isspace(src[j])) /* While it's a space copy the char from src to output */
    {
        output[i] = src[j];
        i++;
        j++;
    }

    output[i] = '\0'; /* Null terminate the string */

    return output;
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

    /* Find the sign */
    if (*p == '+' || *p == '-') 
    {
        sign = *p == '+' ? 1 : -1; 
        p++;
    }

    /* Check if the rest of the string is a number */
    for (i = 0; i < strlen(p); i++)
    {
        if (!isdigit(p[i]))
        {
            return NON_VALID_INTEGER;
        }
    }

    num = atoi(p); /* Parse it */
    return num * sign; /* Return the number with the sign */
}

