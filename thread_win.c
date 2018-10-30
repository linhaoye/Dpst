#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "atomic.h"
#include "thread_win.h"
#include "debug.h"
#include "lfq.h"

#define MAX_THR_NUM 32

static unsigned int __stdcall thread_pool_process(PVOID param) {
  thread_t *thread = (thread_t*) param;
  lfq_t *myq = (lfq_t *)thread->queue;

 void *data = NULL;
  while (thread->parent->status) {
    out:
    if ((data = lfq_deq(myq)) == NULL) {
        if (AT_CAS(thread->stat, BUSY, IDLE)){
        }
        SuspendThread(GetCurrentThread());
        ph_debug("bbbbbbb");
        goto out;
    }

    ph_debug("get!, %x, %d", data, thread->stat);

    if (thread->parent->task != NULL) {
      thread->parent->task(data);
      ph_debug("thread[%d] run task, data=%d, lfq_size=%d!!!", 
        (LONG)thread->thread, *((int*)data), myq->size); 
    }
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
    ph_debug("malloc pool fail!");
    return NULL;
  }
  memset(pool, 0, sizeof(*pool));

  thread_t *threads;
  threads = malloc(size * sizeof(*threads));
  if (threads == NULL) {
    ph_debug("malloc threads fail!");
    return NULL;
  }

  for (i = 0; i < size; i++) {
    lfq_t *lfq = malloc(sizeof(*lfq));
    if (lfq == NULL) {
      ph_debug("malloc lfq fail!");
      return NULL;
    }
    lfq_init(lfq);

    threads[i].queue = lfq;
    threads[i].stat = IDLE;
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
    pool->threads[i].parent = pool;
    pool->threads[i].thread = (HANDLE)_beginthreadex(0, 0, thread_pool_process, 
                                &(pool->threads[i]), 0, 0);
    ph_debug("start POOL?");
  }
}

void thread_pool_dispatch(thread_pool *pool, void *data) {
  int n, i = 0;
  thread_t *thread;

  for (i = 0; i < pool->pool_sz; i++) {
    n = i;
    if (AT_CAS(pool->threads[i].stat, IDLE, IDLE)) {
      ph_debug("take the %d, %d", n, pool->threads[i].stat);
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
  ph_debug("Get the %d", n);

done:
  thread = &(pool->threads[n]);

  if (lfq_enq((lfq_t*)(thread->queue), data) > -1) {
    if (AT_CAS(thread->stat, IDLE, BUSY)) {
      ResumeThread(thread->thread);
    }
  }
}

void thread_pool_end(thread_pool *pool) {
  assert(pool != NULL);
  int i = 0;
  DWORD dw;

  for (i = 0; i<pool->pool_sz; i++) {
    dw = WaitForSingleObject(pool->threads[i].thread, INFINITE);
    switch (dw) {
    case WAIT_OBJECT_0:
      CloseHandle(pool->threads[i].thread);
      break;
    case WAIT_FAILED:
      ph_debug("wait fail!!!");
      break;
    default:
      break;
    }

    lfq_deinit(pool->threads[i].queue);
    free(pool->threads[i].queue);

  }

  free(pool->threads);
  free(pool);
}
