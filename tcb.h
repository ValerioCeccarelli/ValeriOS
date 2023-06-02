#pragma once

#include <stdint.h>
#include "vale_os.h"

typedef struct tcb
{
    uint8_t *sp_save_ptr;
    uint8_t stack[THREAD_STACK_SIZE];

    void (*entry_point)(void);

    int syscall_id;
    int syscall_args[SYSCALL_MAX_ARG_COUNT];
    int syscall_result;

    int8_t pid;

    struct tcb *parent_tcb;

} tcb_t;

void tcb_init(tcb_t *tcb, int8_t pid, tcb_t *parent_tcb, void (*entry_point)(void));