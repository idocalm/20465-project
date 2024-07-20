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
    while (isspace(**p))
    {
        (*p)++;
    }
}

/**
    * Checks if a given string is composed only of spaces.
    * @param start - the start of the string.
    * @param end - the end of the string.
    * @return 1 if the string is empty, 0 otherwise.
*/

int is_empty(char *start, char *end)
{
    while (start < end)
    {
        if (*start != ' ' && *start != '\t')
        {
            return 0; /* not empty */
        }
        start++;
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

/* TOOD: why? */
int non_character(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\0';
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