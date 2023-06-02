#pragma once
#include "list.h"

typedef struct
{
    int id;
    int value;
    list_t waiting_threads;

    int open_count;
} semaphore_t;

void semaphore_init(void);