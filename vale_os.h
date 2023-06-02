#pragma once

#include <stdint.h>

#define SYSTEM_TICKS_PER_SEC 10

#define INTERRUPT_STACK_SIZE 70
#define SYSCALL_STACK_SIZE 200
#define THREAD_STACK_SIZE 200

#define MAX_THREAD_COUNT 4
#define MAX_THREAD_NODE_COUNT (MAX_THREAD_COUNT * 2)

#define SYSCALL_COUNT 12
#define SYSCALL_MAX_ARG_COUNT 2

#define MAX_SEM_COUNT 4
#define MAX_SEM_COUNT_PER_THREAD 4
#define MAX_SEM_NODE_COUNT (MAX_SEM_COUNT * (MAX_THREAD_COUNT + 1)) // +1 for the global semaphore list

void valeos_init(void);
void valeos_start(void);