#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <unistd.h>
#include <pthread.h>

#define ph_assign(p, v) p->set(v, sizeof(*v))

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
  pthread_mutex_t mutex;
  pthread_mutexattr_t attr;
} mutex_t;

#define mutex_init(m, t) do { \
  memset((m), 0, sizeof(*(m))); \
  if ((t) == 1) {\
    pthread_mutex_setpshared(&(m)->attr, PTHREAD_PROCESS_SHARED);\
  }\
  if (pthread_mutex_init(&(m)->mutex, &(m)->attr) < 0) { \
    elog(1, "mutex init error:pthread_mutex_init(%x, %x):%s", \
      &(m)->mutex, &(m)->attr, "-1" \
      );\
  }\
} while(0)

#define mutex_lock(m) pthread_mutex_lock(&(m)->mutex)
#define mutex_unlock(m) pthread_mutex_unlock(&(m)->mutex)

typedef struct {
  worker *workers;
  int pid;
  int num_workers;
  int cur_wk;
  int server_status;
  void* (*set)(void *data, int sz);
  void (*unset)(void *data, int sz);
  void (*task)(void *data);
} process_pool;

void process_pool_envinit();
process_pool* process_pool_new(int num_workers);
void process_pool_start(process_pool *pool);
void process_pool_dispatch(process_pool *pool, void *data);
void process_pool_end(process_pool *pool);

#endif
