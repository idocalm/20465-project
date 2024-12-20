#include "memory.h"
/*
    ------- Memory management  -------
    Implementing this functions allows us to avoid 
    repeating the same error handling process on every allocation 
    around the proj
*/

/**
    * @brief Allocates memory
    * @param size - the size of the memory
    * @return a pointer to the allocated memory


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
    * @param ptr - the pointer to the memory that needs changing
    * @param size - the size of the new memory
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
    * @brief Frees a memory that was allocated.
    * @param ptr - the pointer to the memory

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
    * @param file_name - the name of the file. 
    * @param mode - the reading mode 
    * @return a pointer to the file

*/

FILE *open_file(const char *file_name, char *mode)
{
    FILE *file = fopen(file_name, mode);
    if (file == NULL)
    {
        log_error("System couldn't open file '%s'. Assembler is shutting down.\n", file_name);
        exit(EXIT_FAILURE);
    }
    return file;
}

/**
    * @brief Closes a file. 
    * @param file - the file.

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

