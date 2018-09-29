#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <error.h>
#include "utils.h"

void elog(int fatal, const char *fmt, ...) {
  va_list ap;

#if defined(_NDEDUG)
  if (!fatal) { /* Do not show debug message */
    return;
  }
#endif

  time_t t = time(NULL);
  struct tm *dm = localtime(&t);

  (void) fprintf(stderr,"[%02d:%02d:%02d] :\t", dm->tm_hour,
    dm->tm_min, dm->tm_sec);

  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);

  fputc('\n', stderr);

  if (fatal) {
    exit(EXIT_FAILURE);
  }
}

int listen_s0(int port, uint32_t addr, int backlog) {
  int fd, sz = 1;
  struct sockaddr_in sin;

  if ((fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    return -1;
  }

  blockmode(fd, 0);
  setsocketopt(fd, SOL_SOCKET, SO_REUSEADDR, &sz, sizoef(sz));

  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = htonl(addr);

  if (bind(fd, (struct sockaddr*)&sin, sizoef(sin)) < 0) {
    return -1;
  }
  if (listen(fd, backlog) < 0){
    return -1;
  }

  return fd;
}

int listen_s1(uint16_t port) {
  int sock, on =1, af;

  struct {
      socklen_t   len;
      union {
          struct sockaddr     sa;
          struct sockaddr_in  sin;
      } u;
  #ifdef WITH_IPV6
          struct sockaddr_in6 sin6;
  #endif /* WITH_IPV6 */
  } sa;

#ifdef WITH_IPV6
  af = PF_INET6;
#else
  af = PF_INET;
#endif

  blockmode(sock, 0);
  setsockopt(sock, SOL_SOCKET, SO_REUSEADD, (char*)&on, sizeof(on));

#ifdef WITH_IPV6
  sa.u.sin.sin6_family = af;
  sa.u.sin.sin6_port = htons(port);
  sa.u.sin.sin6_addr = in6addr_any;
  sa.len = sizeof(sa.u.sin6);
#else
  sa.u.sin.sin_family = af;
  sa.u.sin.sin_port = htons(port);
  sa.u.sin.sin_addr.s_addr = INADDR_ANY;
  sa.len = sizeof(sa.u.sin);
#endif

  if (bind(sock, &sa.u.sa, sa.len) < 0) {
    elog(1, "listening: af %d bind(%d):%s", af, port,strerror(ERRNO));
  }

  (void)listen(sock, 16);

  return sock;
}

int blockmode(int fd, int block) {
  int flags;

  if ((flags = fcntl(fd,  F_GETFL, 0)) == -1) {
    elog(0, "nonblock: fcntl(%d, F_GETFL): %s", 
      fd, strerror(ERRNO));
    return -1;
  }

  if (block) {
    flags &= ~O_NONBLOCK;
  } else{
    flags |= ~O_NONBLOCK;
  }

  if (fcntl(fd, F_SETFL, flags) != 0) {
    elog(0, "nonblock: fcntl(%d, F_SETFL): %s", 
      fd, strerror(ERRNO));
    return -1;
  }

  return 0;
}

void *mmap_malloc(size_t sz) {
  void *mem = mmap(
    NULL, 
    sz, 
    PROT_READ | PROT_WRITE,
    MAP_ANON | MAP_SHARED,
    0,
    0
    );
  if (mem < 0) {
    elog(1, "mmap error: mmap(...,%d,...):%s", sz, strerror(ERRNO));
  }
  return mem;
}

void mmap_free(void *p, size_t sz) {
  if (munmap(p, sz) < 0) {
    elog(0, "error: munmap(%x,%d):%s", p, sz, strerror(ERRNO));
  }
}
