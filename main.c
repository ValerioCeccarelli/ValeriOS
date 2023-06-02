#include "uart.h"
#include <vale_os.h>
#include <avr/io.h>
#include <util/delay.h>
#include "tcb.h"
#include "pool_allocator.h"
#include "list.h"
#include "syscall.h"
#include "pid.h"

extern list_t ready_list;
extern pool_allocator_t tcb_node_allocator;
extern pool_allocator_t tcb_allocator;

void f3_func(void);
void f2_func(void);

void f1_func(void)
{
    my_printf("f1_func init\n");
    _delay_ms(1000);
    my_printf("f1_func spawn f2\n");
    int pid2 = syscall_spawn(f2_func);
    int term = syscall_wait(pid2);
    my_printf("f1_func end f2 %d\n", term);

    _delay_ms(1000);
    my_printf("f1_func spawn f3\n");
    int pid3 = syscall_spawn(f3_func);
    _delay_ms(2000);
    int term3 = syscall_wait(pid3);
    my_printf("f1_func end f3 %d\n", term3);

    while (1)
    {
        my_printf("f1_func loop\n");
        _delay_ms(1000);
    }
}

void f2_func(void)
{
    my_printf("f2_func init\n");
    for (int i = 0; i < 3; i++)
    {
        my_printf("f2_func loop\n");
        _delay_ms(1000);
    }
    my_printf("f2_func end f3\n");

    syscall_exit(5);
}

void f3_func(void)
{
    my_printf("f3_func init\n");
    syscall_exit(7);
}

int main(void)
{
    valeos_init();

    my_printf("\n\n\n");
    my_printf("main init\n");

    int pid1 = pid_get();

    tcb_t *tcb1 = (tcb_t *)pool_allocator_allocate(&tcb_allocator);

    list_node_t *tcb_node1 = pool_allocator_allocate(&tcb_node_allocator);

    tcb_node1->data = tcb1;

    tcb_init(tcb1, pid1, 0, f1_func);

    list_enqueue(&ready_list, tcb_node1);

    my_printf("main start\n");

    valeos_start();
    return 0;
}