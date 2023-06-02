#include "uart.h"
#include <vale_os.h>
#include <avr/io.h>
#include "tcb.h"
#include "pool_allocator.h"
#include "list.h"
#include "syscall.h"
#include "pid.h"
#include "util/delay.h"

extern list_t ready_list;
extern pool_allocator_t tcb_node_allocator;
extern pool_allocator_t tcb_allocator;

#pragma region try_idle_thread

// void f1_func(void)
// {
//     my_printf("f1 start\n");
//     while (1)
//     {
//         my_printf("f1\n");
//         syscall_sleep(1000);
//     }
// }

#pragma endregion

#pragma region try_spawn

// void f2_func(void);

// void f1_func(void)
// {
//     my_printf("f1 start\n");

//     for (int i = 0; i < 5; i++)
//     {
//         int pid = syscall_spawn(f2_func);
//         my_printf("f1 spawn %d\n", pid);
//     }

//     while (1)
//     {
//         my_printf("f1 loop\n");
//         syscall_sleep(1000);
//     }
// }

// void f2_func(void)
// {
//     int pid = syscall_getpid();
//     my_printf("f2 start - %d\n", pid);
//     while (1)
//     {
//         my_printf("f2 loop %d\n", pid);
//         syscall_sleep(1000);
//     }
// }

#pragma endregion

#pragma region try_await

// void f2_func(void);
// void f3_func(void);

// void f1_func(void)
// {
//     my_printf("f1 start\n");

//     my_printf("f1 spawn f2\n");
//     int pid2 = syscall_spawn(f2_func);
//     syscall_sleep(1000);
//     my_printf("f1 wait f2\n");
//     int ret2 = syscall_wait(pid2);

//     syscall_sleep(1000);

//     my_printf("f1 spawn f3\n");
//     int pid3 = syscall_spawn(f3_func);
//     my_printf("f1 wait f3\n");
//     int ret3 = syscall_wait(pid3);

//     while (1)
//     {
//         my_printf("f1 loop\n");
//         syscall_sleep(1000);
//     }
// }

// void f2_func(void)
// {
//     my_printf("f2 start\n");
// }

// void f3_func(void)
// {
//     my_printf("f3 start\n");
//     syscall_sleep(1000);
// }

#pragma endregion

#pragma region try_exit_without_parent

void f2_func(void);
void f3_func(void);

void f1_func(void)
{
    my_printf("f1 start\n");

    syscall_spawn(f2_func);

    while (1)
    {
        my_printf("f1 loop\n");
        syscall_sleep(1000);
    }
}

void f2_func(void)
{
    my_printf("f2 start\n");

    syscall_spawn(f3_func);
}

void f3_func(void)
{
    my_printf("f3 start\n");
    for (int i = 0; i < 3; i++)
    {
        my_printf("f3 loop\n");
        syscall_sleep(1000);
    }
}

#pragma endregion

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