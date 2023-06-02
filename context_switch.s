
#include <avr/io.h>

.section .text

.global contextSwitch
contextSwitch:

    push r2
    push r3
    push r4
    push r5
    push r6
    push r7
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    push r16
    push r17
    push r28
    push r29

#ifdef __AVR_3_BYTE_PC__
    in r0,_SFR_IO_ADDR(RAMPZ)
    push r0
    in r0,_SFR_IO_ADDR(EIND)
    push r0
#endif

    in  r16,_SFR_IO_ADDR(SPL)
    in  r17,_SFR_IO_ADDR(SPH)

    mov r28,r24
    mov r29,r25

    st  Y,r16
    std Y+1,r17

    mov r28,r22
    mov r29,r23

    ld r16,Y
    ldd r17,Y+1

    out _SFR_IO_ADDR(SPL),r16
    out _SFR_IO_ADDR(SPH),r17

#ifdef __AVR_3_BYTE_PC__
    pop r0
    in r0,_SFR_IO_ADDR(EIND)
    pop r0
    in r0,_SFR_IO_ADDR(RAMPZ)
#endif

    pop r29
    pop r28
    pop r17
    pop r16
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop r7
    pop r6
    pop r5
    pop r4
    pop r3
    pop r2

    ret

.global setNewContext
setNewContext:

    mov r28,r24
    mov r29,r25

    ld r16,Y
    ldd r17,Y+1

    out _SFR_IO_ADDR(SPL),r16
    out _SFR_IO_ADDR(SPH),r17

#ifdef __AVR_3_BYTE_PC__
    pop r0
    in r0,_SFR_IO_ADDR(EIND)
    pop r0
    in r0,_SFR_IO_ADDR(RAMPZ)
#endif

    pop r29
    pop r28
    pop r17
    pop r16
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop r7
    pop r6
    pop r5
    pop r4
    pop r3
    pop r2

    reti


