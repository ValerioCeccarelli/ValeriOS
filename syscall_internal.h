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