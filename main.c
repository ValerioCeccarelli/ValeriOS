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

#pragma region try_get_pid

// void f1_func(void)
// {
//     int pid = syscall_getpid();
//     my_printf("f1 start with pid %d\n", pid);
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

void f2_func(void);
void f3_func(void);

void f1_func(void)
{
    my_printf("f1 start\n");

    my_printf("f1 spawn f2\n");
    int pid2 = syscall_spawn(f2_func);
    syscall_sleep(1000);
    my_printf("f1 wait f2\n");
    int f2_result = 0;
    int ret2 = syscall_wait(pid2, &f2_result);
    if (ret2)
        my_printf("f1 fail to wait f2\n");
    my_printf("f1 f2 result %d\n", f2_result);

    syscall_sleep(1000);

    my_printf("f1 spawn f3\n");
    int pid3 = syscall_spawn(f3_func);
    my_printf("f1 wait f3\n");
    int f3_result = 0;
    int ret3 = syscall_wait(pid3, &f3_result);
    if (ret3)
        my_printf("f1 fail to wait f3\n");
    my_printf("f1 f3 result %d\n", f3_result);

    while (1)
    {
        my_printf("f1 loop\n");
        syscall_sleep(1000);
    }
}

void f2_func(void)
{
    my_printf("f2 start\n");
    syscall_exit(2);
}

void f3_func(void)
{
    my_printf("f3 start\n");
    syscall_sleep(1000);
    syscall_exit(3);
}

#pragma endregion

#pragma region try_exit_without_parent

// void f2_func(void);
// void f3_func(void);

// void f1_func(void)
// {
//     my_printf("f1 start\n");

//     syscall_spawn(f2_func);

//     while (1)
//     {
//         my_printf("f1 loop\n");
//         syscall_sleep(1000);
//     }
// }

// void f2_func(void)
// {
//     my_printf("f2 start\n");

//     syscall_spawn(f3_func);
// }

// void f3_func(void)
// {
//     my_printf("f3 start\n");
//     for (int i = 0; i < 3; i++)
//     {
//         my_printf("f3 loop\n");
//         syscall_sleep(1000);
//     }
// }

#pragma endregion

#pragma region semaphores

// #define MY_SEM_ID 1

// void f2_func(void);
// void f3_func(void);

// void f1_func(void)
// {
//     my_printf("f1 start\n");

//     int res1 = syscall_sem_create(MY_SEM_ID, 2);
//     if (res1)
//         my_printf("f1 sem create fail\n");

//     int res2 = syscall_sem_open(MY_SEM_ID);
//     if (res2)
//         my_printf("f1 sem open fail\n");

//     int res3 = syscall_spawn(f2_func);
//     my_printf("f1 spawn f2: %d\n", res3);

//     int res7 = syscall_spawn(f3_func);
//     my_printf("f1 spawn f3: %d\n", res7);

//     while (1)
//     {
//         int res4 = syscall_sem_wait(MY_SEM_ID);
//         if (res4)
//             my_printf("f1 sem wait failed\n");
//         my_printf("f1 loop 1\n");
//         syscall_sleep(1000);
//         my_printf("f1 loop 2\n");
//         syscall_sleep(1000);
//         my_printf("f1 loop 3\n");
//         syscall_sleep(1000);
//         int res5 = syscall_sem_post(MY_SEM_ID);
//         if (res5)
//             my_printf("f1 post wait failed\n");
//     }
// }

// void f2_func(void)
// {
//     my_printf("f2 start\n");

//     syscall_sem_open(MY_SEM_ID);

//     while (1)
//     {
//         syscall_sem_wait(MY_SEM_ID);
//         my_printf("f2 loop 1\n");
//         syscall_sleep(1000);
//         my_printf("f2 loop 2\n");
//         syscall_sleep(1000);
//         my_printf("f2 loop 3\n");
//         syscall_sleep(1000);
//         syscall_sem_post(MY_SEM_ID);
//     }
// }

// void f3_func(void)
// {
//     my_printf("f3 start\n");

//     syscall_sem_open(MY_SEM_ID);

//     while (1)
//     {
//         syscall_sem_wait(MY_SEM_ID);
//         my_printf("f3 loop 1\n");
//         syscall_sleep(1000);
//         my_printf("f3 loop 2\n");
//         syscall_sleep(1000);
//         my_printf("f3 loop 3\n");
//         syscall_sleep(1000);
//         syscall_sem_post(MY_SEM_ID);
//     }
// }

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