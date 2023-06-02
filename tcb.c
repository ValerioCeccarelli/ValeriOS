#include "tcb.h"
#include "vale_os.h"
#include <stdio.h>

extern tcb_t *current_tcb;

void _trampoline(void)
{
    // printf("trampoline %d --------------------\n", current_tcb->entry_point);
    current_tcb->entry_point();
}

void tcb_init(tcb_t *tcb, void (*entry_point)(void))
{
    tcb->entry_point = entry_point;

    uint8_t *stack_ptr = tcb->stack + INTERRUPT_STACK_SIZE - 1;

    /** write the return address of the function being called (the trampoline)*/
    *stack_ptr-- = (uint8_t)((uint16_t)_trampoline & 0xFF);
    *stack_ptr-- = (uint8_t)(((uint16_t)_trampoline >> 8) & 0xFF);

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

    tcb->sp_save_ptr = stack_ptr;
}