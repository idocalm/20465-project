#ifndef DATA_LINE
#define DATA_LINE

#include "../structs/list.h"
#include "../structs/labels.h"
#include "../helpers/symbols.h"
#include "../helpers/strings.h"
#include "../helpers/misc.h"
#include "../definitions.h"

/* - Prototypes - */

int insert_data_arguments(char *line, int *dc, machine_word **data_image, int line_num);
void insert_string_arguments(char *line, int *dc, machine_word **data_image, int line_num);
int insert_extern_arguments(char *line, Labels *labels, int line_num);
int handle_data_line(char *line, int line_num, int *dc, char *label, Labels *labels, machine_word **data_image);

#endif