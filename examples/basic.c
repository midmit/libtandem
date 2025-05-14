#include <stdio.h>
#include "tandem/coro.h"

#define STACK_SIZE 64 * 1024

void foo(td_rt *rt) {
  for (int i = 1; i <= 10; i++) {
    printf("FOO: %d\n", i);
    td_suspend(rt);
  }
}

void bar(td_rt *rt) {
  for (int i = 2; i <= 10; i+=2) {
    printf("BAR: %d\n", i);
    td_suspend(rt);
  }
}

int main() {
  td_rt *rt = td_init();

  td_coro* coro1 = td_spawn(rt, &foo, 0, NULL, STACK_SIZE);
  td_coro* coro2 = td_spawn(rt, &bar, 0, NULL, STACK_SIZE);

  while (coro1->status == TD_CORO_RUNNING || coro2->status == TD_CORO_RUNNING) {
    printf("MAIN\n");
    td_resume(rt, coro1);
    td_resume(rt, coro2);
  }

  td_free(rt);

  return 0;
}
