#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "utils.h"
#include "buffer.h"

buffer_t* buf_new(size_t sz) {
  buffer_t *buf = malloc(sizeof(*buf));
  if (buf == NULL) {
    elog(1, "fatal error: malloc(%d)", sizeof(*buf));
  }
  buf_init(buf, sz);

  return buf;
}

void buf_init(buffer_t *buf, size_t sz) {
  assert(buf != NULL);
  memset(buf, 0, sizeof(*buf));

  if (buf->data == NULL) {
    char *data = calloc(sz + 1, 1);
    if (data == NULL) {
      elog(1, "fatal error: calloc(%d, %d)", sz, sizeof(char));
    }
  }

  buf->data = data;
  buf->rdi = 0;
  buf->wti = 0;
  buf->capacity = sz + 1;
}

int buf_write(buffer_t *buf, void* data, size_t n) {
  if (buf_canwrite(buf, n) < 0) {
    return -1;
  }
  memcpy(buf + buf->wti, data, n);

  return 0;
}

int buf_read(buffer_t *buf, void* data, size_t n) {
  if (buf_canread(buf, n) < 0) {
    return -1;
  }



  return 0;
}

int buf_isfull(buffer_t *buf) {
  return ;
}
