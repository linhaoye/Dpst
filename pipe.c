#include <fcntl.h>
#include "utils.h"
#include "pipe.h"

typedef struct _pipe {
  int fd[2];
} _pipe;

void pipe_open(pipe_t *pip, int blocking) {
  _pipe *_pipe = malloc(sizeof (*_pipe));

  if (_pipe == NULL) {
    elog(0, "open pipe: malloc(%d): %s", sizeof(*_pipe), strerror(ERRNO));
  }

  memset(_pipe, 0, sizeof (*_pipe));

  if (pipe(_pipe->fd) < 0) {
    elog(0, "open pipe: pipe(%x): %s", _pipe->fd, strerror(ERRNO));
  }

  if (blocking == 0) {
    blockmode(_pipe->fd[0], O_NONBLOCK);
    blockmode(_pipe->fd[1], O_NONBLOCK);
  }

  pip->object = _pipe;
  pip->blocking = blocking;

  return;
}

int pipe_read(pipe_t *pip, void *data, size_t sz) {
  _pipe *p = pip->object;

  return read(p->fd[0], data, sz);
}

int pipe_write(pipe_t *pip, void *data, size_t sz) {
  _pipe *p = pip->object;

  return write(p->fd[1], data, sz);
}

int pipe_fd(pipe_t *pip, int type) {
  _pipe *p = pip->object;

  return type == 0 ? p->fd[0] : p->fd[1];
}

void pipe_close(pipe_t *pip) {
  _pipe *p = pip->object;

  close(p->fd[0]);
  close(p->fd[1]);
  if (p)  {
    free(p);
  }
}
