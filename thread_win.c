#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "atomic.h"
#include "thread_win.h"
#include "debug.h"
#include "lfq.h"

#define MAX_THR_NUM 32

static unsigned int __stdcall thread_pool_process(PVOID param) {
  thread_t thread = (thread_t*) param;

  void *data = NULL;
  while (thread->parent->status) {
    if ((data = lfq_deq((lfq_t*)thread->queue)) == NULL) {
      if (AT_CAS(thread->stat, BUSY, IDLE)) {
        SuspendThread(thread);
      }
    }

    ph_debug("run task!!!"); 
  }

  return 0;
}

void thread_pool_envinit() {
}

thread_pool* thread_pool_new(size_t size) {
  if (size > MAX_THR_NUM) {
    ph_debug("out of MAX_THR_NUM !!!");
    return NULL;
  }
  thread_pool *pool;
  int i;

  pool =  malloc(sizeof(*pool));
  if (pool == NULL) {
    ph_log_err("fatal error: malloc(%d)!", sizeof(*pool));
    return NULL;
  }
  memset(pool, 0, sizeof(*pool));

  thread_t *threads;
  threads = malloc(size * sizeof(*threads));
  if (threads == NULL) {
    ph_log_err("fatal error: not enough memory!")
    return NULL;
  }

  for (i = 0; i < size; i++) {
    lfq_t *lfq = malloc(sizeof(*lfq));
    if (lfq == NULL) {
      ph_log_err("fatal error: malloc lfq fail!");
      return NULL;
    }
    lfq_init(lfq);

    threads[i]->queue = lfq;
    threads[i]->stat = IDLE;
  }

  pool->pool_sz = size;
  pool->threads = threads;
  pool->counter = 0;
  pool->status = 1;

  return pool;
}

void thread_pool_start(thread_pool *pool) {
  assert(pool != NULL);
  int i;
  for (i = 0; i < pool->pool_sz; i++) {
    pool->threads[i]->thread = (HANDLE)_beginthreadex(0, 0, thread_pool_process, 
                                pool->threads[i], CREATE_SUSPENDED, 0);
    pool->threads[i]->parent = pool;
  }
}

void thread_pool_dispatch(thread_pool *pool, void *data) {
  int n, stat, i = 0;
  thread_t *thread;

  for (i = 0; i < pool->pool_sz; i++) {
    n = i;
    if (AT_CAS(pool->threads[i]->stat, IDLE, BUSY)) { // set it busy
      goto done;
    }
  }

  //average mode
  n = pool->counter;
  if (n >= pool->pool_sz) {
    pool->counter = 0;
    n = 0;
  }
  pool->counter++;

done:
  thread = pool->threads[n];
  lfq_enq((lfq_t*)thread->queue, data);
  ResumeThread(thread);
}

void thread_pool_end(thread_pool *pool) {
  int i = 0;
  HANDLE *handles = malloc(sizeof (HANDLE) * pool->pool_sz);
  if (handles == NULL) {
    ph_log_err("fatal error: malloc handles fail!");
    exit(-1);
  }
  for (i = 0; i < pool->pool_sz; i++) {
    handles[i] = pool->threads[i]->thread;
  }

  DWORD dw = WaitForMultipleObjects(pool->pool_sz; handles, TRUE, INFINITE);
  switch(dw) {
  case WAIT_FAILED:
    ph_debug("invalid handle?");
    break;
  default:
    break;
  }

  for (i = 0; i<pool->pool_sz; i++) {
    lfq_deinit(pool->threads[i]->queue);
    free(pool->threads[i]->queue);

    CloseHandle(pool->threads[i]->thread);
  }

  free(pool->threads);
  free(pool);
  free(handles);
}
