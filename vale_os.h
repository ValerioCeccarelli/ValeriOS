#pragma once

#include <stdint.h>

#define SYSTEM_TICKS_PER_SEC 10

#define INTERRUPT_STACK_SIZE 70
#define SYSCALL_STACK_SIZE 70
#define THREAD_STACK_SIZE 70

#define MAX_THREAD_COUNT 4
#define MAX_THREAD_NODE_COUNT (MAX_THREAD_COUNT * 2)

#define SYSCALL_COUNT 5
#define SYSCALL_MAX_ARG_COUNT 1

void valeos_init(void);
void valeos_start(void);