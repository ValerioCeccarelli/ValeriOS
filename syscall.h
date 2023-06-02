#pragma once

#include <stdint.h>
#include "vale_os.h"

typedef struct
{
    uint8_t *sp_save_ptr;
    uint8_t stack[SYSCALL_STACK_SIZE];
} syscall_tcb_t;

void syscall_init(void);

/// @brief Generic syscall function that can be used to call any syscall.
/// @param syscall_id The ID of the syscall to call
/// @param arg_count The number of arguments to pass to the syscall
/// @param ... The arguments to pass to the syscall
/// @return It returns the value returned by the syscall
int syscall(int syscall_id, int arg_count, ...);

/// @brief This syscall returns the PID of the current process.
int8_t syscall_getpid(void);

/// @brief This syscall creates a new process that starts executing at the given entry point.
/// @param entry_point the entry point of the new thread
/// @return 0 if the thread was successfully created, -1 otherwise
int8_t syscall_spawn(void (*entry_point)(void));

/// @brief This syscall returns the PID of the parent of the current process.
/// @return parent's PID if the current process has a parent alive, -1 otherwise
int8_t syscall_getparentpid(void);

/// @brief This syscall terminates the current process.
/// @param exit_code The exit code of the process
void syscall_exit(int exit_code);

/// @brief This syscall waits for the given process to terminate.
/// (if the process is already terminated, it returns immediately, otherwise it blocks)
/// @param pid The PID of the process to wait for or 0 to wait for any child process
/// @param exit_code The pointer to location to store the exit code of the process
/// @return 0 if the process was successfully waited for, -1 otherwise
int syscall_wait(int8_t pid, int *exit_code);

/// @brief This syscall waits for any child process to terminate.
/// @param exit_code The pointer to location to store the exit code of the process
/// @return 0 if the process was successfully waited for, -1 otherwise
int syscall_wait_any(int *exit_code);

/// @brief This syscall puts the current process to sleep for the given amount of milliseconds.
/// note: the sleep time is approximate, it can be longer than the given time (depending on the system timer frequency)
void syscall_sleep(int ms);

/// @brief This syscall creates a new semaphore with the given initial count.
/// @param id The ID of the semaphore to create
/// @param initial_count The initial count of the semaphore
/// @return 0 if the semaphore was successfully created, -1 otherwise
int syscall_sem_create(int id, int initial_count);

/// @brief This syscall closes the semaphore with the given ID for this process.
/// @param id The ID of the semaphore to close
/// @return 0 if the semaphore was successfully closed, -1 otherwise
int syscall_sem_close(int id);

/// @brief This syscall waits for the semaphore with the given ID.
// (This syscall blocks if there are no more available resources.)
/// @param id The ID of the semaphore to wait for
/// @return 0 if the semaphore was successfully waited for, -1 otherwise
int syscall_sem_wait(int id);

/// @brief This syscall signals the semaphore with the given ID.
/// @param id The ID of the semaphore to signal
/// @return 0 if the semaphore was successfully signaled, -1 otherwise
int syscall_sem_post(int id);

/// @brief This syscall opens the semaphore with the given ID for this process.
/// @param id The ID of the semaphore to open
/// @return 0 if the semaphore was successfully opened, -1 otherwise
int syscall_sem_open(int id);

/// @brief This syscall unlinks the semaphore with the given ID in the system.
/// @param id The ID of the semaphore to unlink
/// @return 0 if the semaphore was successfully unlinked, -1 otherwise
int syscall_sem_unlink(int id);