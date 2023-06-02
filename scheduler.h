#pragma once

#include <stdint.h>

#define IDLE_STACK_SIZE 23

typedef struct
{
    uint8_t *sp_save_ptr;
    uint8_t stack[IDLE_STACK_SIZE];
} idle_tcb_t;

/// @brief Function that initializes the scheduler module.
void scheduler_init(void);

/// @brief Function that starts the scheduler.
/// @note This function never return! It starts the first thread.
void scheduler_start(void);

/// @brief Function that find the next thread to run and sets it as the current thread.
void set_next_current_tcb(void);

/// @brief Function that put the current thread at the end of the ready list.
void remove_current_tcb(void);

/// @brief function that triggers the scheduler that will find the next thread to
/// run and puts the current thread at the end of the ready list.
void schedule(void);