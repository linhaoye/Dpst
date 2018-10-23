#include <stdlib.h>
#include <stdio.h>
#include "atomic.h"
#include "debug.h"
#include "unit.h"

char *atomic_test() {
  int a = 5;
  AT_FAA(a, 1);
  ph_debug("AT_FAA: a=%d", a);
  AT_FAS(a, 1);
  ph_debug("AT_FAS: a=%d", a);
  AT_AAF(a, 1);
  ph_debug("AT_AAF: a=%d", a);
  AT_SAF(a, 1);
  ph_debug("AT_SAF: a=%d", a);
  AT_DEC(a);
  ph_debug("AT_DEC: a=%d", a);
  AT_INC(a);
  ph_debug("AT_INC: a=%d", a);
  ph_debug("AT_LOAD: a=%d", AT_LOAD(a));

  return NULL;
}

char *run_atomic_tests() {
  ph_suite_start();

  ph_run_test(atomic_test);
}

PH_RUN_TESTS(run_atomic_tests);