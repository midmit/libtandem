#ifndef TD_ASYNC_H
#define TD_ASYNC_H

#include "common.h"

void td_sch_init(td_rt *rt);
void td_sch_loop(td_rt *rt);

void td_sleep(td_rt *rt, size_t millis);
ssize_t td_read(td_rt *rt, int fd, void *buf, size_t count);
ssize_t td_write(td_rt *rt, int fd, void *buf, size_t count);

#endif
