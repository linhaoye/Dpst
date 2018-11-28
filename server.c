#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <assert.h>
#include "select.h"
#include "process.h"
#include "buffer.h"
#include "platform.h"
#include "utils.h"

#define MAX_STREAMS 64 * 1024
#define INVALID_SOCKET -1

static update_timeout = 1;
static int max_fds = MAX_STREAMS;
static select_t select_set;

enum {
  STATE_CLOSED,
  STATE_CLOSING,
  STATE_CONNECTING,
  STATE_CONNECTED,
  STATE_LISTENING
};

struct stream {
  int state;
  uint8_t used;
  int sfd;
  int bytes_sent, bytes_received;
  struct sockaddr_in remote_addr;
  buffer_t *iobuf;
  double last_activity, timeout;
  time_t ctime;
  struct stream *next;
};

typedef struct stream stream;

static int stream_count;
static stream *ds_stream, *stream_list;
static mutex_t stream_lock;

void stream_init(void) {
  /*We're unlikely to see an FD much higher than maxstreams.*/
  stream_list = (stream*)s_calloc(max_fds, sizeof(stream));
  if (!stream_list) {
    ph_debug("stream new error!");
    exit(-1);
  }
  int i;
  for (i = 0; i < max_fds; i++) {
    stream_list[i].sfd = INVALID_SOCKET;
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

  s->state = STATE_CLOSED;
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

void stream_cleanup(stream *s) {
  assert(s != NULL);
  if (s->iobuf) {
    buf_free(s->iobuf);
  }
  memset(s, 0, sizeof(*s));
  s->sfd = INVALID_SOCKET;
}

void stream_destroy(stream *s) {
  stream **next;
  if (stream->sfd != INVALID_SOCKET) {
    close(stream->sfd);
  }

  next = &ds_stream;
  while (*next != s) {
    next = &(*next)->next;
  }
  *next = s->next;
  stream_count--;

  /* Destroy and free */
  stream_cleanup(s);
}

int get_stream_count(void) {
  return stream_count;
}

void destory_closed_stream(void) {
  stream *s = ds_stream;

  while (s) {
    if (s->state == STATE_CLOSED) {
      stream *next = s->next;
      stream_destroy(s);
      s = next;
    } else {
      s = s->next;
    }
  }
}

void stream_accept_connection(stream *s) {
  for (;;) {
    stream *remote;
    struct sockaddr_in sin;
    socklen_t len;
    int err = 0, sockfd;

    sockfd = accept(s->sfd, &sin, &len);

    if (sockfd == INVALID_SOCKET) {
      err = ERRNO;
      if (err == EWOULDBLOCK) {
        /* No more waiting sockets */
        return;
      }
    }

    /* Create client stream */
    remote = stream_new(sockfd, 1, &sin, 8192);
    remote->state = STATE_CONNECTED;

    blockmode(remote->sfd, 0);
  }
}

void stream_received_data(stream *s) {
  for (;;) {
    char data[8192];
    int size = ;
  }
}

void srv_init(void) {
  mutex_init(stream_lock, 1);

  signal(SIGPIPE, SIG_IGN);
  stream_init();
}

void srv_update(void) {
  stream *s;
  struct timeval tv;

  mutex_lock(stream_lock);
  destory_closed_stream();
  mutex_unlock(stream_lock);

  /* Create fd sets for select */
  select_zero(&select_set);

  s = ds_stream;
  mutex_lock(stream_lock);
  while (s) {
    switch (s->state) {
    case STATE_CONNECTED:
      select_add(&select_set, SELECT_READ, s->sfd);
      break;

    case STATE_CLOSING:
      select_add(&select_set, SELECT_WRITE, s->sfd);
      break;

    case STATE_CONNECTING:
      select_add(&select_set, SELECT_WRITE, s->sfd);
      select_add(&select_set, SELECT_EXCEPT, s->sfd);
      break;

    case STATE_LISTENING:
      select_add(&select_set, SELECT_READ, s->sfd);
      break;
    }

    s = s->next;
  }
  mutex_unlock(stream_lock);

  tv.tv_sec = update_timeout;
  tv.tv_usec = (update_timeout - tv.tv_sec) * 1e6;

  select(select_set.maxfd + 1,
         select_set.fds[SELECT_READ],
         select_set.fds[SELECT_WRITE],
         select_set.fds[SELECT_EXCEPT],
         &tv
  );

  /* Handle streams */
  s = ds_stream;

  mutex_lock(stream_lock);
  while (s) {
    switch (s->state) {
    case STATE_CONNECTED:
      if (select_has(&select_set, SELECT_READ, s->sfd)) {
        stream_received_data(s);
        if (s->state = STATE_CLOSED) {
          break;
        }
      }
      /* Fall through */

    case STATE_CLOSING:
      break;

    case STATE_CONNECTING:
      break;

    case STATE_LISTENING:
      if (select_has(&select_set, SELECT_READ, s->sfd)) {
        stream_accept_connection(s);
      }
      break;
    }

    s = s->next;
  }
  mutex_unlock(stream_lock);
}


int main(int argc, char* agrv[]) {
  return 0;
}