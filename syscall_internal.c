#include "syscall_internal.h"
#include "tcb.h"
#include "list.h"
#include "pool_allocator.h"
#include "pid.h"
#include <stdint.h>
#include "scheduler.h"
#include "context_switch.h"

extern tcb_t *current_tcb;

extern list_t ready_list;
extern list_t wait_list;

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

    list_node_t *tcb_node = (list_node_t *)pool_allocator_allocate(&tcb_node_allocator);
    tcb_node->data = tcb;
    tcb->status = THREAD_STATUS_READY;
    list_enqueue(&ready_list, tcb_node);

    list_node_t *tcb_child_node = (list_node_t *)pool_allocator_allocate(&tcb_node_allocator);
    tcb_child_node->data = tcb;
    list_enqueue(&current_tcb->children, tcb_child_node);

    current_tcb->syscall_result = pid;
}

void internal_syscall_getparentpid(void)
{
    if (current_tcb->parent_tcb == 0)
    {
        current_tcb->syscall_result = -1;
    }
    else
    {
        current_tcb->syscall_result = current_tcb->parent_tcb->pid;
    }
}

void _remove_or_make_orphan(list_t *list)
{
    list_node_t *node = list->head;
    while (node != 0)
    {
        list_node_t *node_next = node->next;
        tcb_t *tcb = (tcb_t *)node->data;
        if (tcb->status == THREAD_STATUS_EXITED)
        {
            list_remove(list, node);

            pid_free(tcb->pid);
            pool_allocator_free(&tcb_allocator, tcb);
            pool_allocator_free(&tcb_node_allocator, node);
        }
        else
        {
            tcb->parent_tcb = 0;
        }
        node = node_next;
    }
}

void internal_syscall_exit(void)
{
    tcb_t *exited_tcb = current_tcb;
    exited_tcb->exit_code = exited_tcb->syscall_args[0];
    exited_tcb->status = THREAD_STATUS_EXITED;

    // if a process exits
    // if it has children alive, set their parent to -1
    // if it has exited children, free them
    _remove_or_make_orphan(&exited_tcb->children);

    if (exited_tcb->parent_tcb == 0)
    {
        // if I don't have a parent, free myself because no one cares about me
        pid_free(exited_tcb->pid);
        pool_allocator_free(&tcb_allocator, exited_tcb);

        set_next_current_tcb();
        return;
    }
    else
    {
        // if my parent is waiting for me or is waiting for a generic child, wake it up
        if (exited_tcb->parent_tcb->status == THREAD_STATUS_WAITING &&
            (exited_tcb->parent_tcb->syscall_args[0] == 0 ||
             exited_tcb->parent_tcb->syscall_args[0] == exited_tcb->pid))
        {
            exited_tcb->parent_tcb->status = THREAD_STATUS_READY;
            exited_tcb->parent_tcb->syscall_result = exited_tcb->exit_code;
            list_node_t *tcb_node = (list_node_t *)pool_allocator_allocate(&tcb_node_allocator);
            tcb_node->data = exited_tcb->parent_tcb;
            list_enqueue(&ready_list, tcb_node);
        }
    }

    set_next_current_tcb();
}

int8_t _pid_to_find = -1;
int _find_pid(const void *data)
{
    tcb_t *tcb = (tcb_t *)data;
    return tcb->pid == _pid_to_find;
}

int _find_exited(const void *data)
{
    tcb_t *tcb = (tcb_t *)data;
    return tcb->status == THREAD_STATUS_EXITED;
}

void internal_syscall_wait(void)
{
    int8_t pid = (int8_t)current_tcb->syscall_args[0];

    // TODO: what if there is no child?

    if (pid == 0)
    {
        // wait for any child

        list_node_t *child_node = list_find(&current_tcb->children, _find_exited);

        if (child_node == 0)
        {
            current_tcb->status = THREAD_STATUS_WAITING;
            list_node_t *tcb_node = (list_node_t *)pool_allocator_allocate(&tcb_node_allocator);
            tcb_node->data = current_tcb;
            list_enqueue(&wait_list, tcb_node);

            set_next_current_tcb();
        }
        else
        {
            tcb_t *child_tcb = (tcb_t *)child_node->data;
            current_tcb->syscall_result = child_tcb->exit_code;
            list_remove(&current_tcb->children, child_node);

            pid_free(child_tcb->pid);
            pool_allocator_free(&tcb_allocator, child_tcb);
            pool_allocator_free(&tcb_node_allocator, child_node);
        }
    }
    else
    {
        // wait for specific child

        _pid_to_find = pid;
        list_node_t *child_node = list_find(&current_tcb->children, _find_pid);

        // TODO: what if the child is not found? (so there is no such child with the pid)

        tcb_t *child_tcb = (tcb_t *)child_node->data;

        if (child_tcb->status == THREAD_STATUS_EXITED)
        {
            current_tcb->syscall_result = child_tcb->exit_code;
            list_remove(&current_tcb->children, child_node);

            pid_free(child_tcb->pid);
            pool_allocator_free(&tcb_allocator, child_tcb);
            pool_allocator_free(&tcb_node_allocator, child_node);
        }
        else
        {
            current_tcb->status = THREAD_STATUS_WAITING;
            list_node_t *tcb_node = (list_node_t *)pool_allocator_allocate(&tcb_node_allocator);
            tcb_node->data = current_tcb;
            list_enqueue(&wait_list, tcb_node);

            set_next_current_tcb();
        }
    }
}