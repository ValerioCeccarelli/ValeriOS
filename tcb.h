#pragma once

#include <stdint.h>
#include "vale_os.h"
#include "list.h"

typedef enum
{
    THREAD_STATUS_INVALID,
    THREAD_STATUS_READY,
    THREAD_STATUS_WAITING,
    THREAD_STATUS_RUNNING,
    THREAD_STATUS_EXITED,
    THREAD_STATUS_SLEEPING,
} thread_status_t;

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
    list_t children;

    int exit_code;

    thread_status_t status;

    int sleep_until;

    list_t semaphores;
} tcb_t;

void tcb_init(tcb_t *tcb, int8_t pid, tcb_t *parent_tcb, void (*entry_point)(void));