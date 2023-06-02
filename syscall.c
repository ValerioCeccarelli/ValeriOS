#include "syscall.h"
#include "syscall_internal.h"
#include "vale_os.h"
#include <stdarg.h>
#include "context_switch.h"
#include "tcb.h"
#include <avr/interrupt.h>

extern tcb_t *current_tcb;

void (*syscall_table[SYSCALL_COUNT])(void);

syscall_tcb_t syscall_tcb;

void init_syscall_tcb(void);

void reset_syscall_tcb(void)
{
    init_syscall_tcb();
}

void syscall_func(void)
{
    syscall_table[current_tcb->syscall_id]();

    reset_syscall_tcb();
    setNewContext(current_tcb);
}

void init_syscall_tcb(void)
{
    uint8_t *stack_ptr = syscall_tcb.stack + INTERRUPT_STACK_SIZE - 1;

    /** write the return address of the function being called (the trampoline)*/
    *stack_ptr-- = (uint8_t)((uint16_t)syscall_func & 0xFF);
    *stack_ptr-- = (uint8_t)(((uint16_t)syscall_func >> 8) & 0xFF);

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

    syscall_tcb.sp_save_ptr = stack_ptr;
}

void syscall_init(void)
{
    syscall_table[SYSCALL_GETPID] = internal_syscall_getpid;
    syscall_table[SYSCALL_SPAWN] = internal_syscall_spawn;
    syscall_table[SYSCALL_GETPARENTPID] = internal_syscall_getparentpid;
    syscall_table[SYSCALL_EXIT] = internal_syscall_exit;
    syscall_table[SYSCALL_WAIT] = internal_syscall_wait;
    syscall_table[SYSCALL_SLEEP] = internal_syscall_sleep;

    init_syscall_tcb();
}

int syscall(int syscall_id, int arg_count, ...)
{
    current_tcb->syscall_id = syscall_id;

    va_list args;
    va_start(args, arg_count);

    for (int i = 0; i < arg_count; i++)
    {
        current_tcb->syscall_args[i] = va_arg(args, int);
    }

    va_end(args);

    cli();
    contextSwitch(current_tcb, &syscall_tcb);
    sei();

    current_tcb->syscall_id = -1;

    return current_tcb->syscall_result;
}

int8_t syscall_getpid(void)
{
    return (int8_t)syscall(SYSCALL_GETPID, 0);
}

int8_t syscall_spawn(void (*entry_point)(void))
{
    return (int8_t)syscall(SYSCALL_SPAWN, 1, entry_point);
}

int8_t syscall_getparentpid(void)
{
    return (int8_t)syscall(SYSCALL_GETPARENTPID, 0);
}

void syscall_exit(int exit_code)
{
    syscall(SYSCALL_EXIT, 1, exit_code);
}

int syscall_wait(int8_t pid)
{
    return syscall(SYSCALL_WAIT, 1, pid);
}

int syscall_wait_any(void)
{
    return syscall_wait(0);
}

void syscall_sleep(int ms)
{
    syscall(SYSCALL_SLEEP, 1, ms);
}