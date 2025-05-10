#ifndef TD_IO_H
#define TD_IO_H

#include <stdlib.h>

ssize_t td_read(int fd, void *buf, size_t count);
ssize_t td_write(int fd, const void *buf, size_t count);

#endif
