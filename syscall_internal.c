#include "syscall_internal.h"
#include "tcb.h"
#include "list.h"
#include "pool_allocator.h"
#include "pid.h"
#include <stdint.h>

extern tcb_t *current_tcb;

extern list_t ready_list;

extern pool_allocator_t tcb_allocator;
extern pool_allocator_t tcb_node_allocator;

void internal_syscall_getpid(void)
{
    current_tcb->syscall_result = current_tcb->pid;
}

void internal_syscall_spawn(void)
{
    int8_t pid = pid_get();
    if (pid == -1)
    {
        current_tcb->syscall_result = -1;
        return;
    }

    void (*entry_point)(void) = (void (*)(void))current_tcb->syscall_args[0];

    tcb_t *tcb = (tcb_t *)pool_allocator_allocate(&tcb_allocator);
    tcb_init(tcb, pid, current_tcb, entry_point);

    list_node_t *tcb_node = pool_allocator_allocate(&tcb_node_allocator);
    tcb_node->data = tcb;
    list_enqueue(&ready_list, tcb_node);

    current_tcb->syscall_result = pid;
}

void internal_syscall_getparentpid(void)
{
    current_tcb->syscall_result = current_tcb->parent_tcb->pid;
}