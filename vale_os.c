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

#include <stdio.h>

tcb_t *current_tcb;

pool_allocator_t tcb_allocator;
uint8_t tcb_memory[REQUIRED_MEMORY_SIZE(sizeof(tcb_t), MAX_THREAD_COUNT)];

pool_allocator_t tcb_node_allocator;
uint8_t tcb_node_memory[REQUIRED_MEMORY_SIZE(sizeof(list_node_t), MAX_THREAD_NODE_COUNT)];

list_t ready_list;
list_t wait_list;
list_t sleep_list;

void valeos_init(void)
{
    printf_init();
    interrupt_init();
    syscall_init();

    // printf("init allocator\n");

    pool_allocator_init(&tcb_allocator, (uint8_t *)tcb_memory, sizeof(tcb_t), MAX_THREAD_COUNT);
    pool_allocator_init(&tcb_node_allocator, (uint8_t *)tcb_node_memory, sizeof(list_node_t), MAX_THREAD_NODE_COUNT);

    // printf("init lists\n");

    list_init(&ready_list);
    list_init(&wait_list);
    list_init(&sleep_list);
}

void valeos_start(void)
{
    timer_start();
    // printf("timer started\n");

    // this should be the last thing because it will change the context, starting the first thread
    scheduler_start();
}