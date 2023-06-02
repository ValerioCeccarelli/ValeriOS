#pragma once

#include <stdint.h>

typedef struct pool_allocator
{
    uint8_t *memory;
    int8_t *free_list;
    uint32_t block_size;
    int8_t first_free_index;
} pool_allocator_t;

#define REQUIRED_MEMORY_SIZE(block_size, block_count) ((block_size + sizeof(int8_t)) * block_count)

void pool_allocator_init(pool_allocator_t *allocator, uint8_t *memory, uint32_t block_size, uint32_t block_count);
void *pool_allocator_allocate(pool_allocator_t *allocator);
void pool_allocator_free(pool_allocator_t *allocator, void *block);
