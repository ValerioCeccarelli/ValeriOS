#include "scheduler.h"
#include "tcb.h"
#include "context_switch.h"
#include "list.h"
#include "pool_allocator.h"

extern tcb_t *current_tcb;
extern list_t ready_list;
extern pool_allocator_t tcb_node_allocator;

idle_tcb_t idle_tcb;

void _idle_func(void)
{
    while (1)
        ;
}

void scheduler_init(void)
{
    uint8_t *stack_ptr = idle_tcb.stack + INTERRUPT_STACK_SIZE - 1;

    /** write the return address of the function being called (the trampoline)*/
    *stack_ptr-- = (uint8_t)((uint16_t)_idle_func & 0xFF);
    *stack_ptr-- = (uint8_t)(((uint16_t)_idle_func >> 8) & 0xFF);

    // this is for the devices with more than 64K of program memory
#ifdef __AVR_3_BYTE_PC__
    *stack_ptr-- = 0;
#endif

    /**
     * Store starting register values for R2-R17, R28-R29
     */
    *stack_ptr-- = 0x00; /* R2 */
    *stack_ptr-- = 0x00; /* R3 */
    *stack_ptr-- = 0x00; /* R4 */
    *stack_ptr-- = 0x00; /* R5 */
    *stack_ptr-- = 0x00; /* R6 */
    *stack_ptr-- = 0x00; /* R7 */
    *stack_ptr-- = 0x00; /* R8 */
    *stack_ptr-- = 0x00; /* R9 */
    *stack_ptr-- = 0x00; /* R10 */
    *stack_ptr-- = 0x00; /* R11 */
    *stack_ptr-- = 0x00; /* R12 */
    *stack_ptr-- = 0x00; /* R13 */
    *stack_ptr-- = 0x00; /* R14 */
    *stack_ptr-- = 0x00; /* R15 */
    *stack_ptr-- = 0x00; /* R16 */
    *stack_ptr-- = 0x00; /* R17 */
    *stack_ptr-- = 0x00; /* R28 */
    *stack_ptr-- = 0x00; /* R29 */

    /**
     * On devices with large program space we also context switch RAMPZ, EIND.
     */
#ifdef __AVR_3_BYTE_PC__
    *stack_ptr-- = 0x00; /* RAMPZ */
    *stack_ptr-- = 0x00; /* EIND */
#endif

    idle_tcb.sp_save_ptr = stack_ptr;
}

void scheduler_start(void)
{
    set_next_current_tcb();
    setNewContext(current_tcb);
}

void set_next_current_tcb(void)
{
    if (list_is_empty(&ready_list))
    {
        current_tcb = (tcb_t *)&idle_tcb;
    }
    else
    {
        list_node_t *new_tcb_node = list_dequeue(&ready_list);
        tcb_t *new_tcb = (tcb_t *)new_tcb_node->data;

        pool_allocator_free(&tcb_node_allocator, new_tcb_node);

        current_tcb = new_tcb;
        current_tcb->status = THREAD_STATUS_RUNNING;
    }
}

void remove_current_tcb(void)
{
    if (current_tcb == (tcb_t *)&idle_tcb)
    {
        return;
    }
    else
    {
        list_node_t *current_tcb_node = pool_allocator_allocate(&tcb_node_allocator);
        current_tcb_node->data = current_tcb;
        current_tcb->status = THREAD_STATUS_READY;
        list_enqueue(&ready_list, current_tcb_node);
    }
}

void schedule(void)
{
    remove_current_tcb();
    set_next_current_tcb();
}