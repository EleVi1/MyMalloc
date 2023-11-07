#include "allocator.h"

#include <stddef.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

static size_t align(size_t size)
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

struct blk_allocator *blka_new(void)
{
    struct blk_allocator *new = calloc(1, sizeof(struct blk_allocator));
    new->meta = NULL;
    return new;
}

struct blk_meta *blka_alloc(struct blk_allocator *blka, size_t size)
{
    size_t update = align(size);
    if (update == 0)
        update = sysconf(_SC_PAGESIZE);
    struct blk_meta *added = mmap(NULL, update, PROT_READ | PROT_WRITE,
                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (!added)
        return NULL;
    added->size = update - sizeof(struct blk_meta);
    added->next = blka->meta;
    blka->meta = added;
    return added;
}

void blka_free(struct blk_meta *blk)
{
    if (blk != NULL)
        munmap(blk->data, blk->size);
}

void blka_pop(struct blk_allocator *blka)
{
    if (blka)
    {
        struct blk_meta *head = blka->meta;
        if (blka->meta != NULL)
            blka->meta = blka->meta->next;
        blka_free(head);
    }
}

void blka_delete(struct blk_allocator *blka)
{
    free(blka);
}
