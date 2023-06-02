#include <stdio.h>
#include <vale_os.h>
#include <avr/io.h>
#include <util/delay.h>
#include "tcb.h"
#include "pool_allocator.h"
#include "list.h"

extern list_t ready_list;
extern pool_allocator_t tcb_node_allocator;
extern pool_allocator_t tcb_allocator;

void f1_func(void)
{
    printf("f1_func init\n");
    while (1)
    {
        printf("f1_func\n");
        _delay_ms(1000);
    }
}

void f2_func(void)
{
    printf("f2_func init\n");
    while (1)
    {
        printf("f2_func\n");
        _delay_ms(1000);
    }
}

int main(void)
{
    valeos_init();
    _delay_ms(1000);
    printf("\n\n\nmain init\n");
    _delay_ms(1000);
    tcb_t *tcb1 = (tcb_t *)pool_allocator_allocate(&tcb_allocator);
    tcb_t *tcb2 = (tcb_t *)pool_allocator_allocate(&tcb_allocator);

    list_node_t *tcb_node1 = pool_allocator_allocate(&tcb_node_allocator);
    list_node_t *tcb_node2 = pool_allocator_allocate(&tcb_node_allocator);

    tcb_node1->data = tcb1;
    tcb_node2->data = tcb2;

    tcb_init(tcb1, f1_func);
    tcb_init(tcb2, f2_func);

    list_enqueue(&ready_list, tcb_node1);
    list_enqueue(&ready_list, tcb_node2);
    _delay_ms(1000);
    printf("main start\n");
    _delay_ms(1000);
    printf("cosiiii %p %p, %d %d\n", tcb1, tcb2, f1_func, f2_func);
    _delay_ms(1000);
    valeos_start();
    return 0;
}