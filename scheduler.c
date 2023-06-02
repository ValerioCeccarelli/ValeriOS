#include "scheduler.h"
#include "tcb.h"
#include "context_switch.h"
#include "list.h"
#include "pool_allocator.h"

extern tcb_t *current_tcb;
extern list_t ready_list;
extern pool_allocator_t tcb_node_allocator;

void scheduler_start(void)
{
    set_next_current_tcb();
    setNewContext(current_tcb);
}

void set_next_current_tcb(void)
{
    list_node_t *new_tcb_node = list_dequeue(&ready_list);
    tcb_t *new_tcb = (tcb_t *)new_tcb_node->data;

    pool_allocator_free(&tcb_node_allocator, new_tcb_node);

    current_tcb = new_tcb;
}

void remove_current_tcb(void)
{
    list_node_t *current_tcb_node = pool_allocator_allocate(&tcb_node_allocator);
    current_tcb_node->data = current_tcb;
    list_enqueue(&ready_list, current_tcb_node);
}

void schedule(void)
{
    remove_current_tcb();
    set_next_current_tcb();
}