#include "interrupt.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "vale_os.h"
#include <stdint.h>
#include "context_switch.h"
#include "tcb.h"
#include "scheduler.h"
#include "timer.h"

#include <stdio.h>

extern tcb_t *current_tcb;

interrupt_tcb_t interrupt_tcb;
interrupt_tcb_t *interrupt_tcb_p;

void interrupt_func(void)
{
    increment_current_time();

    awake_sleeping_threads();
    schedule();

    // this two functions must be called withouth other functions in between
    reset_interrupt_tcb();
    setNewContext(current_tcb);
}

void reset_interrupt_tcb(void)
{
    interrupt_init();
}

void interrupt_init(void)
{
    uint8_t *stack_ptr = interrupt_tcb.stack + INTERRUPT_STACK_SIZE - 1;

    /** write the return address of the function being called (the trampoline)*/
    *stack_ptr-- = (uint8_t)((uint16_t)interrupt_func & 0xFF);
    *stack_ptr-- = (uint8_t)(((uint16_t)interrupt_func >> 8) & 0xFF);

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

    interrupt_tcb.sp_save_ptr = stack_ptr;

    interrupt_tcb_p = &interrupt_tcb;
}

ISR(TIMER1_COMPA_vect)
{
    contextSwitch(current_tcb, interrupt_tcb_p);
}