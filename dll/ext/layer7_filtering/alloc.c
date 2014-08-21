#include "alloc.h"
#include <stdio.h>

void *WinDivertAlloc(size_t ssize) {
    void *p = malloc(ssize);
    if (p == NULL) {
        printf("WinDivertAlloc() critical error: NO MEMORY!\n");
        exit(1);
    }
    return  p;
}
