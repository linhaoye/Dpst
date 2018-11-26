#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "select.h"
#include "process.h"
#include "buffer.h"
#include "platform.h"

struct conn {
  uint8_t used;
  int fd;
  struct sockaddr_in remote_addr;
  buffer_t *iobuf;
  time_t ctime;
};

vec(struct conn) conn_list;

void conn_new() {
}



int main(int argc, char* agrv[]) {
  return 0;
}