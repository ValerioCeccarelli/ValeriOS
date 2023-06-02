#pragma once

#include <stdint.h>
#include "vale_os.h"

typedef struct
{
    uint8_t *sp_save_ptr;
    uint8_t stack[THREAD_STACK_SIZE];

    void (*entry_point)(void);
} tcb_t;

void tcb_init(tcb_t *tcb, void (*entry_point)(void));