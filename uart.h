#pragma once

#include <stdio.h>

void printf_init(void);
void __attribute__((format(printf, 1, 2))) my_printf(char *format, ...);
