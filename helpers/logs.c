#include "logs.h"

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
    * @brief Changes the text color to blue 
*/
void blue_text()
{
    printf("\033[0;34m");
}

/** 
    * @brief Resets the text color to default 
*/
void reset_text()
{
    printf("\033[0m");
}


/**
    * @brief Logs an error message to the console 
    * @param format - the message 
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
    * @brief Logs a success message to the console 
    * @param format - the message
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
    * @brief Logs a warning message to the console 
    * @param format - the message 
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
    * @brief Logs an info message to the console 
    * @param format - the message
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
    * @brief Logs a debug message to the console 
    * @param format - the message 
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
