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
int8_t syscall_spawn(void (*entry_point)(void));
int8_t syscall_getparentpid(void);
void syscall_exit(int exit_code);
int syscall_wait(int8_t pid);
int syscall_wait_any(void);
void syscall_sleep(int ms);

int syscall_sem_create(int id, int initial_count);
int syscall_sem_close(int id);
int syscall_sem_wait(int id);
int syscall_sem_post(int id);
int syscall_sem_open(int id);
int syscall_sem_unlink(int id);

// TODO: add syscall to get sem value