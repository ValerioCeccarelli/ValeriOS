#pragma once

#include <stdio.h>

/// @brief Function that initializes the print module.
void printf_init(void);

/// @brief Function that prints a string (replaces printf).
/// @param format Format string.
/// @param ... Arguments.
void __attribute__((format(printf, 1, 2))) my_printf(char *format, ...);
