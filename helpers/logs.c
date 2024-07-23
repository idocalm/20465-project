#include "logs.h"

/*
    ------- Formatting output -------
*/

/* Changes the text color to red [for errors] */
void red_text()
{
    printf("\033[0;31m");
}

/* Changes the text color to yellow [for warnings] */
void yellow_text()
{
    printf("\033[0;33m");
}

/* Changes the text color to green [for success messages] */
void green_text()
{
    printf("\033[0;32m");
}


/* Changes the text color to blue */
void blue_text()
{
    printf("\033[0;34m");
}

/* Resets the text color to default */
void reset_text()
{
    printf("\033[0m");
}

/*
    ------- Logging functions -------
*/

/**
    * Logs an error message to the console 
    * @param format - the message (with format specifiers)
    * @param ... - the arguments to be replaced in the format string
 */
void log_error(char *format, ...)
{
    va_list args;
    red_text();
    printf("[ERROR] ");
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    reset_text();
}

/**
    * Logs a success message to the console 
    * @param format - the message (with format specifiers)
    * @param ... - the arguments to be replaced in the format string
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
    * Logs a warning message to the console 
    * @param format - the message (with format specifiers)
    * @param ... - the arguments to be replaced in the format string
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
    * Logs an info message to the console 
    * @param format - the message (with format specifiers)
    * @param ... - the arguments to be replaced in the format string
 */
void log_info(char *format, ...)
{
    va_list args;
    printf("[INFO] ");
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}


/**
    * Logs a debug message to the console 
    * @param format - the message (with format specifiers)
    * @param ... - the arguments to be replaced in the format string
 */
void log_debug(char *format, ...)
{
    va_list args;
    blue_text();
    printf("[DEBUG] ");
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    reset_text();
}