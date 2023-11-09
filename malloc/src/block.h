#ifndef BLOCK_H
#define BLOCK_H

#include <stddef.h>
#include <sys/mman.h>

struct page
{
    struct page *next;
    size_t size;
    size_t page_size;
    size_t capacity;
    void *free;
    void *blocks;
};

struct free_list
{
    struct free_list *next;
};

struct page *create_page(size_t block_size);
size_t align(size_t size);
void *block_allocate(struct page *p);
void page_free(struct page *p);

#endif /* !ALLOCATOR_H */
