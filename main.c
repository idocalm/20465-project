#include <stdio.h>
#include "macros.h"
#include "syntax/symbols.h"

int main(int argc, char *argv[])
{
    int i;

    symbols_init();
    printf("Searching for macros in %d files\n", argc);
    for (i = 1; i < argc; i++)
    {
        search_macros_in_file(argv[i]);
    }
    symbols_free();
    return 0;
}