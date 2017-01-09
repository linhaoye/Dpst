#ifndef __UTILS_H__
#define __UTILS_H__

void elog(int fatal, const char *fmt, ...);
int np_listen(uint16_t port);
int np_blockmode(int fd, int block);
void* np_realloc(void *p, int n);
void* np_malloc(size_t n);
void np_free(void *ptr);

#endif
