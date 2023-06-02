#pragma once

#include <stdint.h>
#include "vale_os.h"

typedef struct
{
    uint8_t *sp_save_ptr;
    uint8_t stack[SYSCALL_STACK_SIZE];
} syscall_tcb_t;

void syscall_init(void);

int syscall(int syscall_id, int arg_count, ...);

int8_t syscall_getpid(void);