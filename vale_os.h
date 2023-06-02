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

/// @brief This function initializes the OS.
void valeos_init(void);

/// @brief Function that sets the first thread to run.
/// @param func Entry point of the first thread.
void valeos_create_first_thread(void (*func)(void));

/// @brief This function starts the OS.
void valeos_start(void);