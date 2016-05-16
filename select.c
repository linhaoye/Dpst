#include <string.h>
#include "utils.h"
#include "io.h"

#define _UNSIGNED_BIT (sizeof(unsigned) * CHAR_BIT)

void select_deinit(SelectSet *s) {
	int i;
	for (i = 0; i <SELECT_MAX; i++) {
		Free(s->fds[i]);
		s->fds[i] = NULL;
	}
	s->capacity = 0;
}

void select_grow(SelectSet *s) {
	int i;
	int oldCapacity = s->capacity;
	s->capacity = s->capacity ? s->capacity << 1 : 1;

	for (i = 0; i < SELECT_MAX; i++) {
		s->fds[i] = Realloc(s->fds[i], s->capacity * sizeof (fd_set));
		memset (s->fds[i] + oldCapacity, 0,
			(s->capacity - oldCapacity) * sizeof (fd_set));
	}
}

void select_zero(SelectSet *s) {
	int i;
	if (s->capacity == 0) return;
	s->maxfd = 0;

	for (i = 0; i < SELECT_MAX; i++) {
		memset(s->fds[i], 0, s->capacity * sizeof(fd_set));
	}
}

void select_add(SelectSet *s, int set, Socket fd) {
	unsigned *p;
	while (s->capacity * FD_SETSIZE < fd) {
		select_grow(s);
	}
	p = (unsigned *) s->fds[set];
	p[fd / _UNSIGNED_BIT] |= 1 << (fd % _UNSIGNED_BIT);
}

int select_has(SelectSet *s, int set, Socket fd) {
	unsigned *p;
	if (s->maxfd < fd) return 0;

	p = (unsigned *) s->fds[set];

	return p[fd / _UNSIGNED_BIT] & ( 1 << (fd % _UNSIGNED_BIT));
}