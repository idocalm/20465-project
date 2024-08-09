#ifndef LOGS_H
#define LOGS_H

#define BOLD "\033[1m"
#define RESET "\033[0m"

#include <stdio.h>
#include <stdarg.h>
#include "../definitions.h"

/* - Prototypes - */

void red_text();
void green_text();
void yellow_text();
void reset_text();


void log_error(char *format, ...);
void log_warning(char *format, ...);
void log_success(char *format, ...);
void log_info(char *format, ...);
void log_debug(char *format, ...);

#endif