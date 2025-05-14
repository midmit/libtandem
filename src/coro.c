#include <stdlib.h>

#include "coro_vec.h"
#include "common.h"

void __td_switch(td_ctx *from, td_ctx *to);
void __td_entry();

td_rt *td_init() {
  td_rt *rt = calloc(1, sizeof(td_rt));
  init_vec(&rt->free);
  init_vec(&rt->running);
  init_vec(&rt->finished);
  init_vec(&rt->dead);

  td_coro coro = {0};
  coro.status = TD_CORO_RUNNING;
  coro.id = 0;

  __td_switch(&coro.ctx, &coro.ctx);

  push_vec(&rt->running, coro);

  rt->current = rt->running.items;

  return rt;
}

void td_free(td_rt *rt) {
  free_vec(&rt->free);
  free_vec(&rt->running);
  free_vec(&rt->finished);
  free_vec(&rt->dead);
  if (rt->sch != NULL) {
    free(rt->sch);
  }
  free(rt);
}

void td_suspend(td_rt *rt) {
  __td_switch(&rt->current->ctx, &rt->running.items->ctx);
}

void td_resume(td_rt *rt, td_coro *coro) {
  if (coro->status == TD_CORO_FINISHED || coro->status == TD_CORO_DEAD)
    return;

  td_coro *current = rt->current;

  // Set the current coro to the child coro
  rt->current = coro;
  __td_switch(&current->ctx, &coro->ctx);
  // Reset the current coro
  rt->current = current;
}

void entry(td_rt *rt) {
  rt->current->fn(rt);

  td_coro coro = unordered_remove_vec(&rt->running, index_of_vec(&rt->running, rt->current));

  rt->current->status = TD_CORO_FINISHED;
  push_vec(&rt->finished, coro);

  td_suspend(rt);
}

size_t td_argc(td_rt *rt) {
  return rt->current->argc;
}

void *td_argv(td_rt *rt) {
  return rt->current->argv;
}

void td_return(td_rt *rt, void *result) {
  rt->current->result = result;
}

void* td_consume(td_rt *rt, td_coro *coro) {
  coro->status = TD_CORO_DEAD;
  td_coro c = unordered_remove_vec(&rt->finished, index_of_vec(&rt->finished, coro));
  push_vec(&rt->dead, c);
  return coro->result;
}

td_coro* td_spawn(td_rt *rt, void(*fn)(td_rt*), size_t argc, void *argv, size_t stack_size) {
  td_coro coro = {0};
  coro.id = ++rt->id_counter;
  coro.argc = argc;
  coro.argv = argv;

  if (rt->free.len > 0) {
    coro = pop_vec(&rt->free);
    if (stack_size > coro.stack_size) {
      coro.stack = realloc(coro.stack, stack_size);
      coro.stack_size = stack_size;
    }
  } else {
    coro.stack = malloc(stack_size);
    coro.stack_size = stack_size;
  }

  coro.status = TD_CORO_RUNNING;
  coro.fn = fn;
  coro.ctx.rip = (void*)__td_entry;
  coro.ctx.rsp = (void*)(coro.stack + stack_size);
  coro.ctx.r12 = (void*)rt;
  coro.ctx.r13 = (void*)entry;

  push_vec(&rt->running, coro);

  return peek_vec(&rt->running);
}
