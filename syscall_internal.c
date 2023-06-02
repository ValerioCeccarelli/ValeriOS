#include "syscall_internal.h"
#include "tcb.h"
#include "list.h"
#include "pool_allocator.h"
#include "pid.h"
#include <stdint.h>
#include "scheduler.h"
#include "context_switch.h"
#include "timer.h"
#include "vale_os.h"
#include "semaphore.h"
#include <stdio.h>

extern tcb_t *current_tcb;

extern list_t ready_list;
extern list_t sleep_list;
extern list_t sem_list;

extern pool_allocator_t tcb_allocator;
extern pool_allocator_t tcb_node_allocator;
extern pool_allocator_t sem_node_allocator;
extern pool_allocator_t sem_allocator;

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

void _remove_or_make_orphan_v(list_node_t *node)
{
    tcb_t *tcb = (tcb_t *)node->data;
    if (tcb->status == THREAD_STATUS_EXITED)
    {
        list_remove(&current_tcb->children, node);

        pid_free(tcb->pid);
        pool_allocator_free(&tcb_allocator, tcb);
        pool_allocator_free(&tcb_node_allocator, node);
    }
    else
    {
        tcb->parent_tcb = 0;
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

    list_foreach(&exited_tcb->children, _remove_or_make_orphan_v);
    // _remove_or_make_orphan(&exited_tcb->children);

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
            *((int *)exited_tcb->parent_tcb->syscall_args[1]) = exited_tcb->exit_code;
            exited_tcb->parent_tcb->syscall_result = 0;

            list_node_t *tcb_node = (list_node_t *)pool_allocator_allocate(&tcb_node_allocator);
            tcb_node->data = exited_tcb->parent_tcb;
            list_enqueue(&ready_list, tcb_node);

            // delete myself
            pid_free(exited_tcb->pid);
            pool_allocator_free(&tcb_allocator, exited_tcb);
        }

        // else, this process is a zombie
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
    int *exit_code = (int *)current_tcb->syscall_args[1];

    // if process has no children, return error
    if (list_is_empty(&current_tcb->children))
    {
        current_tcb->syscall_result = -1;
        return;
    }

    if (pid == 0)
    {
        // wait for any child

        list_node_t *child_node = list_find(&current_tcb->children, _find_exited);

        if (child_node == 0)
        {
            current_tcb->status = THREAD_STATUS_WAITING;
            list_node_t *tcb_node = (list_node_t *)pool_allocator_allocate(&tcb_node_allocator);
            tcb_node->data = current_tcb;

            set_next_current_tcb();
        }
        else
        {
            tcb_t *child_tcb = (tcb_t *)child_node->data;
            *exit_code = child_tcb->exit_code;
            current_tcb->syscall_result = 0;
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

        // if child doesn't exist, return error
        if (child_node == 0)
        {
            current_tcb->syscall_result = -1;
            return;
        }

        tcb_t *child_tcb = (tcb_t *)child_node->data;

        if (child_tcb->status == THREAD_STATUS_EXITED)
        {
            *exit_code = child_tcb->exit_code;
            current_tcb->syscall_result = 0;
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

            set_next_current_tcb();
        }
    }
}

void internal_syscall_sleep(void)
{
    int sleep_time = current_tcb->syscall_args[0];
    sleep_time = sleep_time * SYSTEM_TICKS_PER_SEC / 1000;
    current_tcb->status = THREAD_STATUS_SLEEPING;

    current_tcb->sleep_until = get_current_time() + sleep_time;

    list_node_t *tcb_node = (list_node_t *)pool_allocator_allocate(&tcb_node_allocator);
    tcb_node->data = current_tcb;
    list_enqueue(&sleep_list, tcb_node);

    set_next_current_tcb();
}

int _sem_id_to_find = -1;
int _find_sem(const void *data)
{
    semaphore_t *sem = (semaphore_t *)data;
    return sem->id == _sem_id_to_find;
}
void internal_syscall_sem_create(void)
{
    int sem_id = current_tcb->syscall_args[0];
    int sem_value = current_tcb->syscall_args[1];

    // check if the semaphore already exists
    _sem_id_to_find = sem_id;
    list_node_t *sem_node = list_find(&sem_list, _find_sem);
    if (sem_node != 0)
    {
        current_tcb->syscall_result = -1;
        return;
    }

    // create the semaphore
    semaphore_t *new_sem = (semaphore_t *)pool_allocator_allocate(&sem_allocator);
    if (new_sem == 0)
    {
        current_tcb->syscall_result = -2;
        return;
    }

    // initialize the semaphore
    new_sem->id = sem_id;
    new_sem->value = sem_value;
    new_sem->open_count = 0;

    // add the semaphore to the semaphore list
    sem_node = (list_node_t *)pool_allocator_allocate(&sem_node_allocator);
    sem_node->data = new_sem;
    list_enqueue(&sem_list, sem_node);

    current_tcb->syscall_result = 0;
    return;
}

void internal_syscall_sem_close(void)
{
    int id = current_tcb->syscall_args[0];

    // check if the semaphore exists
    _sem_id_to_find = id;
    list_node_t *sem_node = list_find(&current_tcb->semaphores, _find_sem);
    if (sem_node == 0)
    {
        current_tcb->syscall_result = -1;
        return;
    }

    // notify the semaphore that this thread is no longer using it
    semaphore_t *sem = (semaphore_t *)sem_node->data;
    sem->open_count--;

    // remove the semaphore from the thread's semaphore list
    list_remove(&current_tcb->semaphores, sem_node);
    pool_allocator_free(&sem_node_allocator, sem_node);

    current_tcb->syscall_result = 0;
}

void internal_syscall_sem_wait(void)
{
    int id = current_tcb->syscall_args[0];

    // check if the semaphore exists
    _sem_id_to_find = id;
    list_node_t *sem_node = list_find(&current_tcb->semaphores, _find_sem);
    if (sem_node == 0)
    {
        current_tcb->syscall_result = -1;
        return;
    }

    semaphore_t *sem = (semaphore_t *)sem_node->data;

    sem->value--;

    if (sem->value >= 0)
    {
        // the thread can continue
        current_tcb->syscall_result = 0;
        return;
    }
    else
    {
        // the thread must wait
        current_tcb->status = THREAD_STATUS_WAITING;
        list_node_t *tcb_node = (list_node_t *)pool_allocator_allocate(&tcb_node_allocator);
        tcb_node->data = current_tcb;
        list_enqueue(&sem->waiting_threads, tcb_node);

        current_tcb->syscall_result = 0;

        set_next_current_tcb();
    }
}

void internal_syscall_sem_post(void)
{
    int id = current_tcb->syscall_args[0];

    _sem_id_to_find = id;
    list_node_t *sem_node = list_find(&current_tcb->semaphores, _find_sem);
    if (sem_node == 0)
    {
        current_tcb->syscall_result = -1;
        return;
    }

    semaphore_t *sem = (semaphore_t *)sem_node->data;

    if (!list_is_empty(&sem->waiting_threads))
    {
        list_node_t *tcb_node = list_dequeue(&sem->waiting_threads);
        tcb_t *tcb = (tcb_t *)tcb_node->data;

        tcb->status = THREAD_STATUS_READY;
        tcb_node->data = tcb;
        list_enqueue(&ready_list, tcb_node);
    }

    sem->value++;
}

void internal_syscall_sem_open(void)
{
    int id = current_tcb->syscall_args[0];

    _sem_id_to_find = id;
    list_node_t *sem_node = list_find(&sem_list, _find_sem);
    if (sem_node == 0)
    {
        current_tcb->syscall_result = -1;
        return;
    }

    semaphore_t *sem = (semaphore_t *)sem_node->data;

    sem->open_count++;

    // add the semaphore to the list of semaphores that the current thread has opened
    sem_node = (list_node_t *)pool_allocator_allocate(&sem_node_allocator);
    sem_node->data = sem;
    list_enqueue(&current_tcb->semaphores, sem_node);

    current_tcb->syscall_result = 0;
}

void internal_syscall_sem_unlink(void)
{
    int id = current_tcb->syscall_args[0];

    _sem_id_to_find = id;
    list_node_t *sem_node = list_find(&sem_list, _find_sem);
    if (sem_node == 0)
    {
        current_tcb->syscall_result = -1;
        return;
    }

    semaphore_t *sem = (semaphore_t *)sem_node->data;
    if (sem->open_count > 0)
    {
        current_tcb->syscall_result = -1;
        return;
    }

    list_remove(&sem_list, sem_node);
    pool_allocator_free(&sem_node_allocator, sem_node);
    pool_allocator_free(&sem_allocator, sem);
}