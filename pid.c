#include "pid.h"
#include <stdint.h>
#include "vale_os.h"

int8_t free_pids[MAX_THREAD_COUNT];
int8_t free_pids_count = MAX_THREAD_COUNT;

// TODO: think if pids should be init in reverse order

void pid_init(void)
{
    for (int i = 0; i < MAX_THREAD_COUNT; i++)
    {
        free_pids[i] = i;
    }
}

int8_t pid_get(void)
{
    if (free_pids_count == 0)
    {
        return -1;
    }
    int8_t pid = free_pids[free_pids_count - 1];
    free_pids_count--;
    return pid;
}

void pid_free(int8_t pid)
{
    free_pids[free_pids_count] = pid;
    free_pids_count++;
}