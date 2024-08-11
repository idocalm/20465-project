#include "logs.h"

/*
    A note: According to the forum using a LIMITED amount of global variables is allowed. This is the one & only global var in the whole project.
    The usage here is justified because there are many places in the code where we need to log an error. Usually we won't have the file name in that scope
    because that would force us to pass it to any function that might need to log an error (and there are many!)
*/


char *file_name = NULL;

/**
    * @brief Sets the file name for the logs 
    * @param name - the file name (inclding .as / .am)
*/
void set_file_name(char *name)
{
    file_name = name;
}

/**
    * @brief Changes the text color to red [for errors] 
*/
void red_text()
{
    printf("\033[0;31m");
}

/**
    * @brief Changes the text color to yellow [for warnings] 
*/
void yellow_text()
{
    printf("\033[0;33m");
}

/**
    * @brief Changes the text color to green [for success]
*/
void green_text()
{
    printf("\033[0;32m");
}


/** 
    * @brief Resets the text color to default 
*/
void reset_text()
{
    printf("\033[0m");
}

/**
    * @brief Prints a (simple) error message to the console 
    * @param format - the message 
    * @param ... - the arguments (%d %s ...)
 */
void log_error(char *format, ...)
{
    va_list args;
    red_text(); /* Paint it red*/
    printf("[ERROR] ");
    va_start(args, format); 
    vprintf(format, args);
    va_end(args);
    reset_text(); /* Reset */
}

/**
    * @brief Prints a complete error message to the console, with file name and line num.
    * @param line_num - the line number
    * @param full_line - the full line (so the user could see the error)
    * @param reason - the reason for the error
    * @param ... - the arguments (like in above ) 
 */
void log_line_error(int line_num, char *full_line, char *reason, ...)
{

    /* Format: file_name:line_num: REASON ... */
    /* line_num | LINE */

    va_list args;
    printf(BOLD "%s:%d:"RESET, file_name, line_num);
    red_text();
    printf(" error: ");
    reset_text();
    va_start(args, reason);
    vprintf(reason, args);
    va_end(args);
    printf("\n");

    printf("\t%d | %s\n", line_num, full_line);
}


/**
    * @brief Prints a success message to the console (mainly for a finished stage)
    * @param format - the message
    * @param ... - the arguments
 */
void log_success(char *format, ...)
{
    va_list args;
    green_text();
    printf("[SUCCESS] ");
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    reset_text();
}

/**
    * @brief Prints a warning message to the console 
    * @param format - the message 
    * @param ... - the arguments 
 */
void log_warning(char *format, ...)
{
    va_list args;
    yellow_text();
    printf("[WARNING] ");
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    reset_text();
}


/**
    * @brief Prints an info message to the console 
    * @param format - the message
    * @param ... - the arguments
 */
void log_info(char *format, ...)
{
    va_list args;
    printf("[INFO] ");
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

