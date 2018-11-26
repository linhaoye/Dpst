#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "select.h"
#include "process.h"
#include "buffer.h"
#include "platform.h"
#include "utils.h"

#define MAX_CONN_N 1024

struct conn {
  uint8_t used;
  int fd;
  struct sockaddr_in remote_addr;
  buffer_t *iobuf;
  time_t ctime;
};

struct conn *conn_list;

void conn_new() {
  conn_list = (struct conn*)s_malloc(MAX_CONN_N);
  if (!conn_list) {
    ph_debug("conn new error!");
    exit(-1);
  }
}

struct conn* conn_get(int fd) {
  if (fd > MAX_CONN_N) {
    ph_debug("out of MAX_CONN_N");
    exit(-1);
  }
  return conn_list[fd];
}



int main(int argc, char* agrv[]) {
  return 0;
}