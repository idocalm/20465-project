#include <stdio.h>
#include "macros.h"
#include "syntax/symbols.h"
#include "macros.h"

int main(int argc, char *argv[])
{
    int i;
    MacroResult *res = NULL;
    symbols_init();
    printf("Searching for macros in %d files\n", argc);
    for (i = 1; i < argc; i++)
    {
        res = search_macros_in_file(argv[i]);
        if (res->error == NO_ERROR)
        {
            replace_macros_in_file(argv[i], res->macros);
        }
        else
        {
            fprintf(stderr, "Error in file %s: ", argv[i]);
            return res->error;
        }
    }
    symbols_free();
    return 0;
}