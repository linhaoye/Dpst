#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "utils.h"

void elog(int fatal, const char *fmt, ...) {
	va_list ap;

#if RELEASE
	if (!fatal)
		return;
#endif
	time_t t = time(NULL);
	struct tm *now = localtime(&t);

	(void)fprintf(stderr, "[%02d:%02d:%02d] ", now->tm_hour, now->tm_min, now->tm_sec);

	va_start(ap, fmt);
	(void) vfprintf(stderr, fmt, ap);
	va_end(ap);

	(void) fputc('\n', stderr);

	if (fatal)
		exit(1);
}

void* Realloc(void *ptr, int n) {
	ptr = realloc(ptr, n);

	if (!ptr && n != 0) {
		perror("out of memory");
	}

	return ptr;
}

void* Malloc(size_t n) {
	void *ptr;
	ptr = malloc(n);

	if (!ptr) {
		perror("out of memory");
	}
	bzero(ptr, n);

	return ptr;
}

void Free(void *ptr) {
	free(ptr);
}
