#include <stdio.h>

#include "tandem/coro.h"
#include "tandem/async.h"

#define SEC 1000

int flag = 0;

void input(td_rt *rt) {
  char msg[1024];

  size_t len = td_read(rt, 0, msg, 1024);
  printf("READ: %ld bytes\n", len);

  for (int i = 0; i < len; i++) {
    printf("%c", msg[i]);
  }
  flag = 1;
}

void sleep_loop(td_rt *rt) {
  while (!flag) {
    printf("Waiting...\n");
    td_sleep(rt, SEC);
  }
}

int main() {
  td_rt *rt = td_init();
  td_sch_init(rt);

  td_spawn(rt, &input, 0, NULL, 32 * 1024);
  td_spawn(rt, &sleep_loop, 0, NULL, 32 * 1024);

  td_sch_loop(rt);

  td_free(rt);
  return 0;
}
