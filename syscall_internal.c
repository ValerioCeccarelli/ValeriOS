#include "syscall_internal.h"
#include "tcb.h"

extern tcb_t *current_tcb;

void internal_syscall_getpid(void)
{
    current_tcb->syscall_result = current_tcb->pid;
}