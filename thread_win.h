#ifndef __THREAD_WIN__
#define __THREAD_WIN__

#include <windows.h>
#include <process.h>
#include <stddef.h>

enum thread_stat {
  BUSY,
  IDLE
};

typedef struct {
  int stat;
  int exit;
  HANDEL thread;
  void *queue;
  PVOID param;
  CriticalSectionLock qlock;
  thread_pool *parent;
} thread_t;

typedef struct {
  thread_t *threads;
  int status;
  int counter;
  void (*task)(void* data);
  size_t pool_sz;
} thread_pool;

void thread_pool_envinit();
thread_pool* thread_pool_new(size_t size);
void thread_pool_start(thread_pool *pool);
void thread_pool_dispatch(thread_pool *pool, void *data);
void thread_pool_end(thread_pool *pool);

#endif
