#pragma once

#include <stdint.h>

/// @brief Function that initializes the PID module.
void pid_init(void);

/// @brief Function that gets a PID.
/// @return The PID or -1 if no PID is available.
int8_t pid_get(void);

/// @brief Function that frees a PID.
void pid_free(int8_t pid);