#pragma once

/// @brief Function that initializes the timer module.
void timer_start(void);

/// @brief Function that weakes up sleeping threads if their sleep_until time has passed.
void awake_sleeping_threads(void);

/// @brief Function that gets the system current time.
int get_current_time(void);

/// @brief Function that increments the system current time.
void increment_current_time(void);