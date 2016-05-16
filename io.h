#ifndef __IO_H__
#define __IO_H__

typedef int Socket;

enum {
	SELECT_READ,
	SELECT_WRITE,
	SELECT_EXCEPT,
	SELECT_MAX
};

typedef struct {
	int capacity;
	Socket maxfd;
	fd_set *fds[SELECT_MAX];
} SelectSet;


/* select */
void select_deinit(SelectSet *s);
void select_grow(SelectSet *s);
void select_zero(SelectSet *s);
void select_add(SelectSet *s, int set, Socket fd);
int select_has(SelectSet *s, int set, Socket fd);


#endif