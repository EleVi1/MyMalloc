#include "beware_overflow.h"

#include <stddef.h>

void *beware_overflow(void *ptr, size_t nmemb, size_t size)
{
    unsigned char *p_bis = ptr;
    size_t res = 0;
    if (__builtin_mul_overflow(nmemb, size, &res) == 0)
    {
        p_bis += res;
        ptr = p_bis;
        return ptr;
    }
    else
        return NULL;
}
