#include "strings.h"
/*
    ------- Strings operations -------
*/

/**
    * Skips all non-spaces at the beginning of a given string.
    * @param p - the pointer to the string.
*/
void skip_non_spaces(char **pp_line) {
    while (**pp_line && !isspace(**pp_line)) {
        (*pp_line)++; 
    }
}



/**
    * Skips all spaces at the beginning of a given string.
    * @param p - the pointer to the string.
*/
void skip_spaces(char **p)
{
    while (**p != '\0' && isspace(**p))
    {
        (*p)++;
    }
}

/**
    * Checks if a given string is composed only of spaces.
    * @param str - the string.
    * @return 1 if the string is empty, 0 otherwise.
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
    * Trims all spaces from the beginning and ending of a string.
    * @param p - the pointer to the string.
*/

void remove_all_spaces(char *p) {
    char *p_start = p;
    char *p_end = p + strlen(p) - 1;

    while (isspace(*p_start)) {
        p_start++;
    }

    while (isspace(*p_end)) {
        p_end--;
    }

    strncpy(p, p_start, p_end - p_start + 1);
    p[p_end - p_start + 1] = '\0';
}

/**
    * Copies a string from the source to the destination until a space is found.
    * @param dest - the destination string.
    * @param src - the source string.
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
    * Checks if a given string is a comment.
    * @param p_line - the pointer to the string.
    * @return 1 if the string is a comment, 0 otherwise.
*/

int is_comment(char *p_line) {
    skip_spaces(&p_line);
    return *p_line == COMMENT_PREFIX; 
}

/**
    * Checks if a given string is an integer that can be represented in 12 bits.
    * @param p - the pointer to the string.
    * @return the integer value if the string is an integer, 
 */

int is_integer(char *p)
{

    /*
        Note that an integer in our system can include a '+' or '-' sign 
        at the beginning.
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
