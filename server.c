#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "select.h"
#include "process.h"
#include "buffer.h"
#include "platform.h"
#include "utils.h"

#define MAX_STREAMS 64 * 1024

static update_timeout = 1;
static int max_fds = MAX_STREAMS;
static select_t select_set;

enum {
  STATE_CLOSED,
  STATE_CLOSING,
  STATE_streamECTING,
  STATE_streamECTED,
  STATE_LISTENING
};

struct stream {
  int state;
  uint8_t used;
  int sfd;
  int bytes_received;
  struct sockaddr_in remote_addr;
  buffer_t *iobuf;
  time_t ctime;
  struct stream *next;
};

typedef struct stream stream;

static int stream_count;
static stream *ds_stream;
static stream *stream_list;

void stream_init(void) {
  /*We're unlikely to see an FD much higher than maxstreams.*/
  stream_list = (stream*)s_calloc(max_fds, sizeof(stream));
  if (!stream_list) {
    ph_debug("stream new error!");
    exit(-1);
  }
}

stream* stream_new(int sfd, uint8_t used,
                struct sockaddr_in *sin, size_t buf_sz) {
  assert(sfd > 0);

  if (fd > max_fds) {
    ph_debug("out of max streams!");
    exit(-1);
  }
  stream *s;
  s = &stream_list[sfd];

  s->used = used;
  s->sfd = sfd;
  s->iobuf = buf_new(buf_sz);
  s->ctime = time(NULL);

  memcpy(&s->remote_addr, sin, sizeof(*sin));

  /* Add to list an increment count */
  s->next = ds_stream;
  ds_stream = s;
  stream_count++;

  return s;
}

void stream_cleanup(stream *c) {
  assert(c != NULL);
  if (c->iobuf) {
    buf_free(c->iobuf);
  }
  memset(c,0, sizeof(*c));
}

int get_stream_count(void) {
  return stream_count;
}

void dspat_update(void) {
}


int main(int argc, char* agrv[]) {
  int i, lsn, quit = 0;
  struct timeval tv;

  stream_init();
  select_zero(&set);

  tv.tv_sec = update_timeout;
  tv.tv_usec = (update_timeout - tv.tv_sec) * 1e6;

  while(quit != 0) {
    select(select_set.maxfd + 1,
           select_set.fds[SELECT_READ],
           select_set.fds[SELECT_WRITE],
           select_set.fds[SELECT_EXCEPT],
           &tv);
  }

  return 0;
}