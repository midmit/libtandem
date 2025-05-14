#include <stdlib.h>
#include <time.h>
#include <liburing.h>
#include <assert.h>

#include "common.h"
#include "coro_vec.h"
#include "tandem/coro.h"

#ifndef TD_STACK_SIZE
#define TD_STACK_SIZE 128 * 1024 // 128kb
#endif

#ifndef TD_RING_SIZE
#define TD_RING_SIZE 32
#endif

typedef void (*fn)(td_rt*);

void td_sch_loop(td_rt *rt) {
  while (rt->running.len > 1) {
    bool all_waiting = true;
    do {
      // Assume all coros are waiting on io

      all_waiting = true;
      // Loop through all coros and resume if its RUNNING
      for (int i = 1; i < rt->running.len; i++) {
        td_coro *coro = rt->running.items + i;
        if (coro->status == TD_CORO_RUNNING) {
          td_resume(rt, coro);

          // Coro is not waiting on io (bare suspend)
          if (coro->status == TD_CORO_RUNNING) {
            all_waiting = false;
          }
        }
      }
      // Wait till all coros are waiting for io or there are no active coros.
    } while (!all_waiting);

    for (int i = 1; i < rt->running.len; i++) {
      struct io_uring_cqe *cqe;
      io_uring_wait_cqe(&rt->sch->ring, &cqe);

      // TODO: Pass this cqe to somewhere
      io_uring_cqe_seen(&rt->sch->ring, cqe);

      size_t id = (size_t)io_uring_cqe_get_data(cqe);
      td_coro *coro = NULL;

      for (int i = 1; i < rt->running.len; i++) {
        if ((rt->running.items + i)->id == id) {
          coro = rt->running.items + i;
        }
      }

      assert(coro != NULL);

      rt->scratch = cqe;
      td_resume(rt, coro);
    }
  }

  // TODO: Cleanup is the scheduler's job.
}

void td_sch_init(td_rt *rt) {
  td_sch *sch = malloc(sizeof(td_sch));
  struct io_uring ring;

  io_uring_queue_init(TD_RING_SIZE, &ring, 0);

  sch->ring = ring;
  rt->sch = sch;
}

void *sch_id(td_rt *rt) {
  return (void*)rt->current->id;
}

void sch_wait_on_io(td_rt *rt, struct io_uring_cqe **cqe) {
  rt->current->status = TD_CORO_WAITING_ON_IO;
  td_suspend(rt);
  *cqe =(struct io_uring_cqe*)rt->scratch;
  rt->current->status = TD_CORO_RUNNING;
}

void td_sleep(td_rt *rt, size_t millis) {
  assert(&rt->sch != NULL);

  struct __kernel_timespec ts = { .tv_sec = 0, .tv_nsec = millis * 1000000 };
  struct io_uring_sqe *sqe = io_uring_get_sqe(&rt->sch->ring);
  struct io_uring_cqe *cqe;

  assert(sqe != NULL);

  io_uring_prep_timeout(sqe, &ts, 0, 0);
  io_uring_sqe_set_data(sqe, sch_id(rt));
  io_uring_submit(&rt->sch->ring);

  sch_wait_on_io(rt, &cqe);
}


ssize_t td_read(td_rt *rt, int fd, void *buf, size_t count) {
  assert(&rt->sch != NULL);

  struct io_uring_sqe *sqe = io_uring_get_sqe(&rt->sch->ring);
  struct io_uring_cqe *cqe;

  assert(sqe != NULL);

  io_uring_prep_read(sqe, fd, buf, count, 0);
  io_uring_sqe_set_data(sqe, sch_id(rt));
  io_uring_submit(&rt->sch->ring);

  sch_wait_on_io(rt, &cqe);

  return cqe->res;
}

ssize_t td_write(td_rt *rt, int fd, void *buf, size_t count) {
  assert(&rt->sch != NULL);

  struct io_uring_sqe *sqe = io_uring_get_sqe(&rt->sch->ring);
  struct io_uring_cqe *cqe;

  assert(sqe != NULL);

  io_uring_prep_write(sqe, fd, buf, count, 0);
  io_uring_sqe_set_data(sqe, sch_id(rt));
  io_uring_submit(&rt->sch->ring);

  sch_wait_on_io(rt, &cqe);

  return cqe->res;
}
