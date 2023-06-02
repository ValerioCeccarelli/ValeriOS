#include "vale_os.h"
#include "uart.h"
#include "tcb.h"
#include "pool_allocator.h"
#include "list.h"
#include "timer.h"
#include "scheduler.h"
#include <stdint.h>
#include "interrupt.h"
#include "syscall.h"
#include "pid.h"
#include "semaphore.h"

#include <stdio.h>

tcb_t *current_tcb;

pool_allocator_t tcb_allocator;
uint8_t tcb_memory[REQUIRED_MEMORY_SIZE(sizeof(tcb_t), MAX_THREAD_COUNT)];

pool_allocator_t tcb_node_allocator;
uint8_t tcb_node_memory[REQUIRED_MEMORY_SIZE(sizeof(list_node_t), MAX_THREAD_NODE_COUNT)];

list_t ready_list;
list_t sleep_list;

void valeos_init(void)
{
    printf_init();
    interrupt_init();
    syscall_init();
    pid_init();
    scheduler_init();
    semaphore_init();

    pool_allocator_init(&tcb_allocator, (uint8_t *)tcb_memory, sizeof(tcb_t), MAX_THREAD_COUNT);
    pool_allocator_init(&tcb_node_allocator, (uint8_t *)tcb_node_memory, sizeof(list_node_t), MAX_THREAD_NODE_COUNT);

    list_init(&ready_list);
    list_init(&sleep_list);
}

void valeos_start(void)
{
    timer_start();

    // this should be the last thing because it will change the context, starting the first thread
    scheduler_start();
}

void valeos_create_first_thread(void (*func)(void))
{
    int pid1 = pid_get();

    tcb_t *tcb1 = (tcb_t *)pool_allocator_allocate(&tcb_allocator);
    tcb_init(tcb1, pid1, 0, func);

    list_node_t *tcb_node1 = pool_allocator_allocate(&tcb_node_allocator);
    tcb_node1->data = tcb1;
    list_enqueue(&ready_list, tcb_node1);
}