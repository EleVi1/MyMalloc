#include "page_begin.h"

#include <stddef.h>

void *page_begin(void *ptr, size_t page_size)
{
    unsigned char *res = NULL;
    size_t p = (size_t)ptr & (~(page_size - 1));
    return res += p;
}
