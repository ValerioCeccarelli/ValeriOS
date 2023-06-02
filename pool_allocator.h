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

/// @brief Function that initializes the pool allocator.
/// @param allocator The allocator to initialize.
/// @param memory The memory to use for the allocator.
/// @param block_size The size of each block of interest.
/// @param block_count Number of blocks of interest.
void pool_allocator_init(pool_allocator_t *allocator, uint8_t *memory, uint32_t block_size, uint32_t block_count);

/// @brief Function that allocates a block of memory.
/// @param allocator The allocator to use.
/// @return A pointer to the allocated block or 0 if no block is available.
void *pool_allocator_allocate(pool_allocator_t *allocator);

/// @brief Function that frees a block of memory.
/// @param allocator The allocator to use.
/// @param block The block to free.
void pool_allocator_free(pool_allocator_t *allocator, void *block);
