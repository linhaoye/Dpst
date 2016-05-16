#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdlib.h>

void elog(int fatal, const char *fmt, ...);
void* Realloc(void *ptr, int n);
void* Malloc(size_t n);
void Free(void *ptr);

#endif
