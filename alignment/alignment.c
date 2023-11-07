#include "alignment.h"

#include <stdio.h>

size_t align(size_t size)
{
    size_t offset = size % sizeof(long double);
    if (offset == 0)
        return size;
    size_t res;
    if (__builtin_add_overflow(size, 0, &res) != 0)
        return 0;
    size_t block = (size / sizeof(long double) + 1) * sizeof(long double);
    return block;
}

/*int main(void)
{
    printf("size = %zu\n", align());
    return 0;
}
*/
