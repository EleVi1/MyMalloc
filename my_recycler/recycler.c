#include "recycler.h"

#include <stdlib.h>

struct recycler *recycler_create(size_t block_size, size_t total_size)
{
    if (block_size == 0 || total_size == 0 || block_size % sizeof(size_t) != 0
        || total_size % block_size != 0)
        return NULL;
    struct recycler *new = malloc(sizeof(struct recycler));
    if (!new)
        return NULL;
    new->block_size = block_size;
    new->capacity = total_size;
    new->chunk = malloc(total_size * sizeof(size_t));
    if (!new->chunk)
    {
        free(new);
        return NULL;
    }
    new->free = new->chunk;
    struct free_list *liste = new->free;
    for (size_t i = 1; i < total_size / block_size; i++)
    {
        liste->next = liste + block_size;
        liste = liste->next;
    }
    liste->next = NULL;
    return new;
}

void recycler_destroy(struct recycler *r)
{
    if (r)
    {
        free(r->chunk);
        free(r);
    }
}

void *recycler_allocate(struct recycler *r)
{
    if (!r || r->free == NULL)
        return NULL;
    struct free_list *tmp = r->free;
    void *res = tmp;
    r->free = tmp->next;
    return res;
}

void recycler_free(struct recycler *r, void *block)
{
    if (r != NULL || block != NULL)
    {
        struct free_list *block = block;
        block->next = r->free;
        r->free = block;
    }
}
