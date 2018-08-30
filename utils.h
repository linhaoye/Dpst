#ifndef __UTILS_H__
#define __UTILS_H__

void elog(int fatal, const char *fmt, ...);
void emsg(const char *fmt, ...)
int listening(uint16_t port);
int blockmode(int fd, int block);
void *mmap_malloc(size_t sz);
void mmap_free(void *p, size_t sz);

#endif
