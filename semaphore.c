#include "semaphore.h"
#include "list.h"
#include "pool_allocator.h"
#include "vale_os.h"

pool_allocator_t sem_allocator;
uint8_t sem_memory[REQUIRED_MEMORY_SIZE(sizeof(semaphore_t), MAX_SEM_COUNT)];

pool_allocator_t sem_node_allocator;
uint8_t sem_node_memory[REQUIRED_MEMORY_SIZE(sizeof(list_node_t), MAX_SEM_NODE_COUNT)];

list_t sem_list;

void semaphore_init(void)
{
    pool_allocator_init(&sem_allocator, (uint8_t *)sem_memory, sizeof(semaphore_t), MAX_SEM_COUNT);
    pool_allocator_init(&sem_node_allocator, (uint8_t *)sem_node_memory, sizeof(list_node_t), MAX_SEM_COUNT);
    list_init(&sem_list);
}
