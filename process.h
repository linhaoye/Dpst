#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <unistd.h>
#include <pthread.h>

#define ASSIGN(P, V) P->set(V, sizeof(*V))

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
} mut_t;

#define MUT_INIT(M, T) do { \
  memset(M, 0, sizeof(*M)); \
  if (T == 1) {\
    pthread_mutex_setpshared(&M->attr, PTHREAD_PROCESS_SHARED);\
  }\
  if (pthread_mutex_init(&M->mutex, &M->attr) < 0) { \
    elog(1, "mutex init error:pthread_mutex_init(%x, %x):%s", \
      &M->mutex, &M->attr, "-1" \
      );\
  }\
} while(0)

#define MUT_LOCK(M) pthread_mutex_lock(&M->mutex)

#define MUT_UNLOCK(M) pthread_mutex_unlock(&M->mutex)

typedef struct {
  worker *workers;
  int pid;
  int num_workers;
  int cur_wk;
  int server_status;
  void* (*set)(void *data, int sz);
  void (*unset)(void *data, int sz);
} process_pool;

void process_pool_envinit();
process_pool* process_pool_new(int num_workers);
void process_pool_start(process_pool *pool);
void process_pool_dispatch(process_pool *pool, void *data);
void process_pool_end(process_pool *pool);

#endif
