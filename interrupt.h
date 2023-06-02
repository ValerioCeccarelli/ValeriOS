#pragma once

#include <stdint.h>
#include "vale_os.h"

typedef struct
{
    uint8_t *sp_save_ptr;
    uint8_t stack[INTERRUPT_STACK_SIZE];
} interrupt_tcb_t;

/// @brief Function that initializes the interrupt module.
void interrupt_init(void);