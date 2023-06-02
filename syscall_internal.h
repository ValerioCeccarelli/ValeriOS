#pragma once

#define SYSCALL_GETPID 0
void internal_syscall_getpid(void);

#define SYSCALL_SPAWN 1
void internal_syscall_spawn(void);

#define SYSCALL_GETPARENTPID 2
void internal_syscall_getparentpid(void);