#ifndef CORO_VEC_H
#define CORO_VEC_H

#include <stdlib.h>
#include <assert.h>
#include <string.h> // memmove()

#include "common.h"

#define INITIAL_SIZE 4
#define SCALING_FACTOR 2

void init_vec(coro_vec *vec) {
  vec->items = malloc(sizeof(td_coro)*INITIAL_SIZE);
  vec->len = 0;
  vec->capacity = 4;
};

void free_vec(coro_vec *vec) {
  free(vec->items);
}

void push_vec(coro_vec *vec, td_coro coro) {
  if (vec->capacity == vec->len) {
    vec->items = realloc(vec->items, vec->capacity * SCALING_FACTOR);
    vec->capacity = vec->capacity * SCALING_FACTOR;
  }
  *(vec->items + vec->len++) = coro;
}

td_coro* peek_vec(coro_vec *vec) {
  return vec->items + (vec->len - 1);
}

td_coro pop_vec(coro_vec *vec) {
  td_coro coro = *(vec->items + --vec->len);
  return coro;
}


size_t index_of_vec(coro_vec *vec, td_coro *coro) {
  for (int i = 0; i < vec->len; i++) {
    if ((vec->items + i) == coro) {
      return i;
    }
  }
  return -1;
}

td_coro ordered_remove_vec(coro_vec *vec, size_t index) {
  assert(vec->len > index);
  td_coro coro = *(vec->items + index);
  memmove(vec->items + index, vec->items + index + 1, --vec->len - index * sizeof(td_coro));
  return coro;
}

td_coro unordered_remove_vec(coro_vec *vec, size_t index) {
  assert(vec->len > index);
  td_coro coro = *(vec->items + index);
  *(vec->items + index) = *(vec->items + --vec->len);
  return coro;
}

#endif
