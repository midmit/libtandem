#ifndef TD_CORO_H
#define TD_CORO_H

#include "common.h"
#include <stdlib.h>

td_rt *td_init();
void td_free(td_rt*);

td_coro *td_spawn(td_rt*, void (*fn)(td_rt*), size_t stack_size);

extern void td_suspend(td_rt*);
extern void td_resume(td_rt*, td_coro*);
#endif
