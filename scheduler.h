#pragma once

#include <stdint.h>

#define IDLE_STACK_SIZE 23

typedef struct
{
    uint8_t *sp_save_ptr;
    uint8_t stack[IDLE_STACK_SIZE];
} idle_tcb_t;

void scheduler_init(void);
void scheduler_start(void);
void set_next_current_tcb(void);
void remove_current_tcb(void);
void schedule(void);