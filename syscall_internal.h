#pragma once

#define SYSCALL_GETPID 0
void internal_syscall_getpid(void);

#define SYSCALL_SPAWN 1
void internal_syscall_spawn(void);

#define SYSCALL_GETPARENTPID 2
void internal_syscall_getparentpid(void);

#define SYSCALL_EXIT 3
void internal_syscall_exit(void);

#define SYSCALL_WAIT 4
void internal_syscall_wait(void);

#define SYSCALL_SLEEP 5
void internal_syscall_sleep(void);

#define SYSCALL_SEM_CREATE 6
void internal_syscall_sem_create(void);

#define SYSCALL_SEM_CLOSE 7
void internal_syscall_sem_close(void);

#define SYSCALL_SEM_WAIT 8
void internal_syscall_sem_wait(void);

#define SYSCALL_SEM_POST 9
void internal_syscall_sem_post(void);

#define SYSCALL_SEM_OPEN 10
void internal_syscall_sem_open(void);

#define SYSCALL_SEM_UNLINK 11
void internal_syscall_sem_unlink(void);