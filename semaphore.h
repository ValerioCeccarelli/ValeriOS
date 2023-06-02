#pragma once
#include "list.h"

typedef struct
{
    int id;
    int value;
    list_t waiting_threads;

    int open_count;
} semaphore_t;

/// @brief Function that initializes the semaphore module.
void semaphore_init(void);