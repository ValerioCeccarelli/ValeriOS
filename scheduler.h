#pragma once

void scheduler_start(void);
void set_next_current_tcb(void);
void remove_current_tcb(void);
void schedule(void);