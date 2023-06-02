#pragma once

// prototype for assembly functions

/// @brief This function saves the context of the current thread in the old_tcb_ptr and restores
/// the context of the new thread from the new_tcb_ptr.
/// @param old_tcb_ptr Pointer to the old tcb stak pointer.
/// @param new_tcb_ptr Pointer to the new tcb stack pointer.
void contextSwitch(void *old_tcb_ptr, void *new_tcb_ptr);

/// @brief This function restores the context of the first thread.
/// (This completly replaces the current context, so the current context is lost)
void setNewContext(void *new_tcb_ptr);
