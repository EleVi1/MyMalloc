#include "malloc.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "block.h"

struct page *first = NULL;

size_t new_size(size_t size)
{
    size_t res = 16;
    while (res < size)
    {
        if (__builtin_mul_overflow(res, 2, &res) != 0)
        {
            return 0;
        }
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
        return block_allocate(first);
    }
    struct page *tmp = first;
    while (tmp)
    {
        if (tmp->size == size)
        {
            if (tmp->free != NULL)
            // if (tmp->capacity > 0)
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

static void *page_begin(void *ptr, size_t page_size)
{
    unsigned char *res = NULL;
    size_t p = (size_t)ptr & (~(page_size - 1));
    return res += p;
}

__attribute__((visibility("default"))) void free(void *ptr)
{
    if (ptr != NULL)
    {
        struct page *tmp = first;
        struct free_list *freed;
        void *begin;
        size_t cap;
        struct page *prev = NULL;
        while (tmp) // Going through the pages
        {
            begin = page_begin(ptr, tmp->page_size);
            if (begin == (void *)tmp)
            {
                freed = ptr;
                freed->next = tmp->free;
                tmp->free = freed;
                tmp->capacity++;
                cap = (tmp->page_size - sizeof(struct page)) / tmp->size;
                if (cap == tmp->capacity) // Empty
                {
                    if (prev)
                        prev->next = tmp->next;
                    else
                        first = first->next;
                    munmap(tmp, tmp->page_size);
                }
                return;
            }
            prev = tmp;
            if (tmp)
            {
                tmp = tmp->next;
            }
            else
            {
                return;
            }
        }
    }
}

__attribute__((visibility("default"))) void *realloc(void *ptr, size_t size)
{
    if (ptr == NULL)
        return malloc(size);
    if (size == 0 && ptr != NULL)
    {
        free(ptr);
        return NULL;
    }
    size = new_size(size);
    if (ptr)
    {
        struct page *tmp = first;
        void *begin;
        while (tmp) // Going through the pages
        {
            begin = page_begin(ptr, tmp->page_size);
            if (begin == (void *)tmp)
            {
                if (tmp->size > size)
                {
                    return ptr;
                }
                begin = malloc(size);
                memcpy(begin, ptr, tmp->size);
                free(ptr);
                return begin;
            }
            if (tmp)
            {
                tmp = tmp->next;
            }
            else
            {
                return ptr;
            }
        }
    }
    return NULL;
}

// Same as malloc excepted
// - Detect overflows
// - Memset the block before returning it
__attribute__((visibility("default"))) void *calloc(size_t nmemb, size_t size)
{
    size_t new = 0;
    if (__builtin_mul_overflow(size, nmemb, &new))
    {
        fprintf(stderr, "calloc: overflow of nmemb * size\n");
        return NULL;
    }
    size_t aligned = new_size(nmemb * size);
    if (aligned == 0)
        return NULL;
    void *res = malloc(aligned);
    res = memset(res, 0, aligned);
    return res;
}
