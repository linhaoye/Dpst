#ifndef __UTILS_H__
#define __UTILS_H__

void elog(int fatal, const char *fmt, ...);
int listening(uint16_t port);
int blockmode(int fd, int block);

#endif
