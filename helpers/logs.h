#ifndef LOGS_H
#define LOGS_H

#define BOLD "\033[1m"
#define RESET "\033[0m"

#include <stdio.h>
#include <stdarg.h>
#include "../definitions.h"

/* - Prototypes - */

void set_file_name(char *name);

void red_text();
void yellow_text();
void green_text();
void reset_text();

void log_error(char *format, ...);
void log_line_error(int line_num, char *full_line, char *reason, ...);
void log_success(char *format, ...);
void log_warning(char *format, ...);
void log_info(char *format, ...);

#endif