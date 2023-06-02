#pragma once

#include <stdint.h>
#include "vale_os.h"

typedef struct
{
    uint8_t *sp_save_ptr;
    uint8_t stack[INTERRUPT_STACK_SIZE];
} interrupt_tcb_t;

void interrupt_init(void);
void reset_interrupt_tcb(void);
void interrupt_func(void);