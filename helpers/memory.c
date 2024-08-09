#include "memory.h"
/*
    ------- Memory management  -------
*/

/**
    * @brief Allocates memory
    * @param size - the size of the memory
    * @return a pointer to the allocated memory

    Implementing this function allows us to avoid 
    repeating the same error handling process on every allocation. 

*/
void *safe_malloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL)
    {
        log_error("[SYSTEM PROBLEM] Memory allocation of size: %ld has failed. Assembler is shutting down. [SYSTEM PROBLEM]\n", size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}


/**
    * @brief Reallocates memory of a given size
    * @param ptr - the pointer to the memory to be reallocated
    * @param size - the size of the memory to be allocated
    * @return a pointer to the reallocated memory

*/

void *safe_realloc(void *ptr, size_t size)
{
    void* temp = realloc(ptr, size);
    if (temp == NULL)
    {
        free(ptr);
        log_error("[SYSTEM PROBLEM] Memory re-allocation of size: %ld has failed. Assembler is shutting down. [SYSTEM PROBLEM]\n", size);
        exit(EXIT_FAILURE);
    }
    return temp;
}



/**
    * @brief Frees the memory allocated.
    * @param ptr - the pointer to the memory to be freed

*/

void safe_free(void *ptr)
{
    if (ptr == NULL)
    {
        log_warning("Trying to free NULL pointer\n");
        return;
    }
    free(ptr);
}

/*
    ------- File management  -------
*/

/**
    * @brief Opens a file. 
    * @param filename - the name of the file. 
    * @param mode - the mode in which the file should be read.
    * @return a pointer to the opened file

*/

FILE *open_file(const char *filename, char *mode)
{
    FILE *file = fopen(filename, mode);
    if (file == NULL)
    {
        log_error("System couldn't open file '%s'. Assembler is shutting down.\n", filename);
        exit(EXIT_FAILURE);
    }
    return file;
}

/**
    * @brief Closes a file. 
    * @param file - the file to be closed.

*/
void close_file(FILE *file)
{
    if (file == NULL)
    {
        log_warning("Trying to close NULL file\n"); /* For debugging... You shouldn't run into this in practice! (Hopefully) */
        return;
    }
    fclose(file);
}

/**
    * @brief Frees the code image 
    * @param code_image - the code image
    * @param ic - the instruction counter
*/

void free_code_image(machine_word **code_image, int ic)
{
    
    int i = 0;
    for (i = 0; i < ic - INITIAL_IC_VALUE; i++)
    {
        safe_free(code_image[i]);
    }
}

/**
    * @brief Frees the data image 
    * @param data_image - the data image
    * @param dc - the data counter
*/

void free_data_image(machine_word **data_image, int dc)
{
    int i = 0;
    for (i = 0; i < dc; i++)
    {
        safe_free(data_image[i]);
    }
}

/**
    * @brief Frees the memory of the operands array
    * @param operands - the array of operands
    * @param operands_count - the number of operands
*/
void free_operands(char **operands, int operands_count) {
    int i = 0;

    for (; i < operands_count; i++) {
        safe_free(operands[i]);
    }
    safe_free(operands);
}
