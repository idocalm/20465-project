#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "structs/linkedlist.h"
#include "structs/hash_table.h"

typedef enum {
    NO_FPASS_ERROR
} FPassErrors;





FPassErrors first_pass(char *p_fileName, long *p_ic, long *p_dc, ht_t *p_data_image, ht_t *p_code_image, ht_t *p_macros);


#endif 