#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <unistd.h>

typedef struct {
  char buf[2048];
  int fd;
} job_t;

typedef struct {
  void *queue;
  pid_t pid;
  int pipe_fd;
} worker;

typedef struct {
  worker *workers;
  int pid;
  int num_workers;
  int cur_wk;
} process_pool;

void process_pool_init(process_pool *pool, int num_workers);
void process_pool_start(process_pool *pool);
void process_pool_dispatch(process_pool *pool, void *data)
void process_pool_end(process_pool *pool);

#endif
