#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>

void elog(int fatal, const char *fmt, ...);
int listen_s0(int port, uint32_t addr, int backlog);
int listen_s1(uint16_t port);
int blockmode(int fd, int block);
void *mmap_malloc(size_t sz);
void mmap_free(void *p, size_t sz);

#endif
