#ifndef CORO_VEC_H
#define CORO_VEC_H

#include <stdlib.h>
#include <assert.h>
#include <string.h> // memmove()

#include "common.h"

#define INITIAL_SIZE 4
#define SCALING_FACTOR 2

void init_vec(coro_vec *vec);
void free_vec(coro_vec *vec);

void push_vec(coro_vec *vec, td_coro coro);

td_coro pop_vec(coro_vec *vec);

td_coro *peek_vec(coro_vec *vec);

size_t index_of_vec(coro_vec *vec, td_coro *coro);

td_coro ordered_remove_vec(coro_vec *vec, size_t index);

td_coro unordered_remove_vec(coro_vec *vec, size_t index);

#endif
