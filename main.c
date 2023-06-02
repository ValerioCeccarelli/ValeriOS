#include <stdio.h>
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

void f1_func(void)
{
    printf("f1_func init\n");

    for (int i = 0; i < 3; i++)
    {
        printf("f1_func --- %d\n", i);
        _delay_ms(1000);
    }

    for (int i = 0; i < 7; i++)
    {
        int pid = syscall_spawn(f3_func);
        printf("f1_func spawned %d\n", pid);
        _delay_ms(1000);
    }

    printf("f1_func done\n");

    while (1)
    {
        int pid = syscall_getpid();
        printf("f1_func %d\n", pid);
        _delay_ms(1000);
    }
}

void f2_func(void)
{
    printf("f2_func init\n");
    while (1)
    {
        int pid = syscall_getpid();
        printf("f2_func %d\n", pid);
        _delay_ms(1000);
    }
}

void f3_func(void)
{
    printf("f3_func init\n");
    while (1)
    {
        int pid = syscall_getpid();
        printf("f3_func %d\n", pid);
        _delay_ms(1000);
    }
}

int main(void)
{
    valeos_init();

    printf("\n\n\n");
    printf("main init\n");

    int pid1 = pid_get();
    int pid2 = pid_get();

    tcb_t *tcb1 = (tcb_t *)pool_allocator_allocate(&tcb_allocator);
    tcb_t *tcb2 = (tcb_t *)pool_allocator_allocate(&tcb_allocator);

    list_node_t *tcb_node1 = pool_allocator_allocate(&tcb_node_allocator);
    list_node_t *tcb_node2 = pool_allocator_allocate(&tcb_node_allocator);

    tcb_node1->data = tcb1;
    tcb_node2->data = tcb2;

    tcb_init(tcb1, pid1, 0, f1_func);
    tcb_init(tcb2, pid2, 0, f2_func);

    list_enqueue(&ready_list, tcb_node1);
    list_enqueue(&ready_list, tcb_node2);

    printf("main start\n");

    valeos_start();
    return 0;
}