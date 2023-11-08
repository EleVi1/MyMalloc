#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "block.h"
#include "malloc.h"

struct page *first = NULL;

static size_t new_size(size_t size)
{
    size_t res = 1;
    while (res < size)
    {
        res *= 16;
    }
    return res;
}

__attribute__((visibility("default"))) void *malloc(size_t size)
{
    if (size == 0)
        return NULL;
    size = new_size(size);
    // No page
    if (!first)
    {
        first = create_page(size);
        printf("Capacity : %zu\n", first->capacity);
        return block_allocate(first);
    }
    struct page *tmp = first;
    while(tmp)
    {
        if (tmp->size == size)
        {
            if (tmp->capacity > 0)
            {
                return block_allocate(tmp);
            }
        }
        if (tmp->next == NULL)
            break;
        tmp = tmp->next;
    }
    tmp->next = create_page(size);
    return block_allocate(tmp->next);
}

__attribute__((visibility("default"))) void free(void *ptr)
{}

__attribute__((visibility("default"))) void *realloc(void *ptr, size_t size)
{
    return NULL;
}

__attribute__((visibility("default"))) void *calloc(size_t nmemb, size_t size)
{
    return NULL;
}
