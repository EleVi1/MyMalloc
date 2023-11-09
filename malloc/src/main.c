#include "block.h"
#include "malloc.h"

#include <stdio.h>

struct page *first = NULL;

/*static size_t new_size(size_t size)
{
    size_t offset = size % sizeof(long double);
    if (offset == 0)
        return size;
    size_t res;
    if (__builtin_add_overflow(size, 0, &res) != 0)
        return 0;
    size_t block = (size / sizeof(long double) + 1) * sizeof(long double);
    return block;
}*/

int main(void)
{
    char *s = malloc(128);
    char *s2 = malloc(128);
    /*for (int i = 0; i < 15; i++)
    {
        printf("%c\n", s[i]);
    }*/
    free(s2);
    free(s);
    return 0;
}
