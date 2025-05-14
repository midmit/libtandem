#ifndef TD_COMMON_H
#define TD_COMMON_H

#include <stdlib.h>
#include <liburing.h>

typedef enum td_coro_status {
  TD_CORO_FREE,
  TD_CORO_RUNNING,
  TD_CORO_WAITING_ON_IO,
  TD_CORO_FINISHED,
  TD_CORO_DEAD,
} td_coro_status;

typedef struct td_ctx {
  void *rip, *rsp, *rbp, *rbx, *r12, *r13, *r14, *r15;
} td_ctx;


typedef struct coro_vec {
  struct td_coro *items;
  size_t len;
  size_t capacity;
} coro_vec;


typedef struct td_sch {
  struct io_uring ring;
} td_sch;

typedef struct td_rt {
  size_t id_counter;
  coro_vec free;
  coro_vec running;
  coro_vec finished;
  coro_vec dead;
  void *scratch;
  struct td_sch *sch;
  struct td_coro *current;
} td_rt;

typedef struct td_coro {
  size_t id;
  td_ctx ctx;
  void* stack;
  size_t stack_size;
  td_coro_status status;
  void (*fn)(td_rt*);

  size_t argc;
  void *argv;
  void *result;

} td_coro;

#endif
