#include <stdlib.h>
#include "include/malloc.h"

int main()
{
    malloc(1024 * 1024 * 128);
    malloc(1024 * 1024 * 16);
    malloc(1024 * 1024);
    malloc(1024 * 32);
    malloc(1024);
    malloc(512);
    show_alloc_mem();
    show_alloc_mem_hex();
    return (0); 
}