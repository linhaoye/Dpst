#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>

#ifndef __cplusplus
typedef int bool;
#define true  1
#define false 0
#endif

void elog(int fatal, const char *fmt, ...);
int listen_s0(int port, uint32_t addr, int backlog);
int listen_s1(uint16_t port);
int blockmode(int fd, int block);
void *s_malloc(size_t sz);
void s_free(void *p, size_t sz);

#endif
