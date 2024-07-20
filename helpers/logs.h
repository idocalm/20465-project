#ifndef LOGS_H
#define LOGS_H

#include <stdio.h>
#include <stdarg.h>

void red_text();
void green_text();
void yellow_text();
void reset_text();


void log_error(char *format, ...);
void log_warning(char *format, ...);
void log_success(char *format, ...);
void log_info(char *format, ...);

#endif