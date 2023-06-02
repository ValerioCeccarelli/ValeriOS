#pragma once

#include <stdint.h>

void pid_init(void);
int8_t pid_get(void);
void pid_free(int8_t pid);