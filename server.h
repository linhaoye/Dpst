#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/types.h>
#include "io.h"

/* global tag */
struct {
	short status;
	short process_type;
} SG;

enum {
	EVENT_ACCEPT,
	EVENT_LISTEN,
	EVENT_LINE
};

typedef struct Event {
	int type;
	void *udata;
	const char *msg;
	char *data;
	int size;
}; 

typedef void (*CallbacK_t)(Event *);

typedef struct {
	int event;
	CallbacK_t callback;
	void *udata;
} Listener;

typedef struct Stream {
	int state;
	int flags;
	Socket sockfd;
	int bytes_send;
	int bytes_recv;
	double last_activity;
	double timeout;
	Vec(Listener) Listeners;
	struct Stream *next;
} Stream;

typedef struct Watcher {
	pthread_t ptid;
	SelectSet selectset;
};

ssize_t nwrite(int fd, const void *buf, size_t count);
ssize_t nread(int fd, const void *buf, size_t count);
int tcp_server(int port, uint32_t, int backlog);

#endif
