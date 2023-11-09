#include "block.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "malloc.h"

size_t align(size_t size)
{
    size_t offset = size % sysconf(_SC_PAGESIZE);
    if (offset == 0)
        return size;
    size_t res;
    if (__builtin_add_overflow(size, 0, &res) != 0)
        return 0;
    size_t block = (size / sysconf(_SC_PAGESIZE) + 1) * sysconf(_SC_PAGESIZE);
    return block;
}

struct page *create_page(size_t block_size)
{
    size_t update = align(block_size + sizeof(struct page));
    if (update == 0)
        update = sysconf(_SC_PAGESIZE);
    struct page *added = mmap(NULL, update, PROT_READ | PROT_WRITE,
                              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (!added)
        return NULL;
    added->page_size = update;
    added->size = block_size;
    added->capacity = (update - sizeof(struct page)) / block_size;
    added->blocks = added;
    char *blk = added->blocks;
    blk += sizeof(struct page);
    added->blocks = blk;
    added->free = added->blocks;
    struct free_list *l = added->free;
    for (size_t i = 1; i < added->capacity; i++)
    {
        l->next = l + block_size / sizeof(size_t);
        l = l->next;
    }
    l->next = NULL;
    added->next = NULL;
    return added;
}

void *block_allocate(struct page *p)
{
    if (!p || p->free == NULL)
        return NULL;

    struct free_list *tmp = p->free;
    p->free = tmp->next;
    p->capacity--;
    return tmp;
}

void page_free(struct page *p)
{
    if (p != NULL)
        munmap(p, p->size + sizeof(struct page));
}
