#include "macros.h"
#include "syntax/symbols.h"

int main(int argc, char *argv[]) {
    int i;

    symbols_init();
    for (i = 0; i < argc; i++) {
        search_macros_in_file(argv[i]);
    }    
    symbols_free();
}