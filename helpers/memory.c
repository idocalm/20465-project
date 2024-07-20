#include "memory.h"
/*
    ------- Memory management  -------
*/

/**
    * Allocates memory of a given size
    * @param size - the size of the memory to be allocated
    * @return a pointer to the allocated memory

    Implementing this function allows us to avoid 
    repeating the same error handling process on every allocation. 

*/
void *safe_malloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL)
    {
        log_error("Could not allocate memory of size: %ld\n", size);
        exit(1);
    }
    return ptr;
}


/**
    * Reallocates memory of a given size
    * @param ptr - the pointer to the memory to be reallocated
    * @param size - the size of the memory to be allocated
    * @return a pointer to the reallocated memory

*/


void *safe_realloc(void *ptr, size_t size)
{
    ptr = realloc(ptr, size);
    if (ptr == NULL)
    {
        log_error("Could not reallocate memory of size: %ld\n", size);
        exit(1);
    }
    return ptr;
}


/**
    * Frees the memory allocated.
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
    * Opens a file. 
    * @param filename - the name of the file. 
    * @param mode - the mode in which the file should be read.
    * @return a pointer to the opened file

*/

FILE *open_file(const char *filename, char *mode)
{
    FILE *file = fopen(filename, mode);
    if (file == NULL)
    {
        log_error("Could not open file %s\n", filename);
        exit(1);
    }
    return file;
}

/**
    * Closes a file. 
    * @param file - the file to be closed.

*/
void close_file(FILE *file)
{
    if (file == NULL)
    {
        log_warning("Trying to close NULL file\n");
        return;
    }
    fclose(file);
}