#ifndef TD_CORO_H
#define TD_CORO_H

#include "common.h"
#include <stdlib.h>

td_rt *td_init();
void td_free(td_rt*);

// Spawn a coroutine
td_coro *td_spawn(td_rt*, void (*fn)(td_rt*), size_t argc, void *argv, size_t stack_size);

// Suspend a coroutine back to the main coroutine where `td_init` was called.
// Nothing happens when called from the main coroutine.
void td_suspend(td_rt*);

// Resume a paused, waiting coroutine.
void td_resume(td_rt*, td_coro*);

size_t td_argc(td_rt*);
void *td_argv(td_rt*);

// Set the return value
void td_return(td_rt*, void *result);

// Consume the return value.
// Also marks the coroutine as DEAD.
void* td_consume(td_rt*, td_coro*);

#endif
