#include "pool_allocator.h"

#include <stdint.h>
#include "uart.h"
#include <avr/interrupt.h>

void pool_allocator_init(pool_allocator_t *allocator, uint8_t *memory, uint32_t block_size, uint32_t block_count)
{
    allocator->memory = memory;
    allocator->block_size = block_size;
    allocator->free_list = (int8_t *)(memory + block_count * block_size);
    allocator->first_free_index = 0;
    for (int i = 0; i < block_count - 1; i++)
    {
        allocator->free_list[i] = i + 1;
    }
    allocator->free_list[block_count - 1] = -1;
}
void *pool_allocator_allocate(pool_allocator_t *allocator)
{
    if (allocator->first_free_index == -1)
    {
        return 0;
    }
    uint8_t *block = allocator->memory + allocator->first_free_index * allocator->block_size;
    int8_t next_free_index = allocator->free_list[allocator->first_free_index];
    allocator->free_list[allocator->first_free_index] = -2;
    allocator->first_free_index = next_free_index;
    return block;
}
void pool_allocator_free(pool_allocator_t *allocator, void *block)
{
    uint8_t *block_ptr = (uint8_t *)block;
    int8_t block_index = (block_ptr - allocator->memory) / allocator->block_size;
    allocator->free_list[block_index] = allocator->first_free_index;
    allocator->first_free_index = block_index;
}
