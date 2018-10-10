#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"
#include "unit.h"

char *buffer_tests() {
  buffer_t *buf = NULL;
  int i;
  char mbuf[10]="helloworld";

  buf = buf_new(1024);
  for(i=0; i<10; i++) {
    mbuf[8] = i;
    mbuf[9] = '\0';
    buf_write(buf, mbuf, 10);
  }
  memcpy(mbuf, 0, sizeof(mbuf));
  for(i=0; i<10; i++) {
  	buf_read(buf, mbuf, 10);
    ph_debug("%s", mbuf);
  }

  buf_free(buf);
  return NULL;
}

char *run_all_tests() {
  ph_suite_start();
  ph_run_test(buffer_tests);

  return NULL;
}

PH_RUN_TESTS(run_all_tests);
