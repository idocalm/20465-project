/* skip_spaces - move the pointer to the first non-space character */
void skip_spaces(char **p) {
    while (**p == ' ' || **p == '\t') {
        (*p)++;
    }
}

FILE *open_file(const char *fileName, const char *mode) {
    FILE *file = fopen(fileName, mode);
    if (file == NULL) {
        /* TODO: Check forum if program should exit */ 
        fprintf(stderr, "[ERROR]: Could not open file %s\n", fileName);
        exit(1);
    }
    return file;
}

void *alloc(size_t size) {
    void *p = malloc(size);
    if (p == NULL) {
        /* TODO: Check forum if program should exit */ 
        fprintf(stderr, "[ERROR]: Memory allocation failed\n");
        exit(1);
    }
    return p;
}

