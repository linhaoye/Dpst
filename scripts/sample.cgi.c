#include <stdio.h>
#include <stdlib.h>
#include "../debug.h"
#include "../dao.h"
#include "cgic.h"

int cgiMain() {
  cgiHeaderContentType("text/html;charset=UTF-8");
  fprintf(stdout, "%s", "<h1>abcde</h1>");
  fprintf(stdout, "%s\n", cgiQueryString);
  return 0;
}
