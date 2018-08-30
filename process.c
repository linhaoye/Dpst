#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "process.h"

/**
 * global process mamanger object
 */
static struct {
  uint8_t process_type; //process type
  int reloading; //
  int whether_reload;
} ctx;

int *pool_status; //process pool status

/* process tag */
enum {
  MASTER_PROCESS,
  WORKER_PROCESS,
  MANAGE_PROCESS
};

typedef struct {
  int pipe[2];
} Pipe;

static int spawn(process_pool *pool, int n) {
  int i, pid; 
  process_pool *p = pool;

  pid = fork();
  if (pid < 0) {
    elog(0,"spawn worker: fork(): %s", strerror(ERRNO));
    return -1;
  } else if (pid == 0) {
    for (i = 0; i < p->num_workers; i++) {
      if (n != i) {
        close(p->workers[i].pipe_fd);
      }
    }
    ctx.process_type = WORKER_PROCESS;
    worker_process(p, p->workers[n].pipe_fd);
    return 0;
  } else {
    return pid;
  }
}

static void sigusr1(int signo) {
  ctx.reloading = 1;
  ctx.whether_reload = 0;
}

static void sigterm(int signo) {
  *pool_status = 0;
}

static void worker_process(process_pool *pool, int p) {
  int n;
  pid_t pid;
  job_t jobs;

  pid = getpid();
  while (1) {
    n = read(p, &jobs, sizeof(job_t));
    if (n > 0) {
      emsg("worker[%d] recv jobs: buf=%s | fd=%d\n", pid, jobs.buf, jobs.fd);
    }
  }
}

static void manager_process(process_pool *pool) {
  int i, n, sz;
  pid_t pid;
  worker *workers;

  (void) signal(SIGUSR1, sigusr1);

  n = 0;
  sz = pool->num_workers * sizeof(worker);
  workers = malloc(sz);
  if (workers == NULL) {
    elog(1, "mamanger process: malloc(%d): %s", sz, strerror(ERRNO));
  }

  while (1) {
    pid = wait(NULL);

    if (pid < 0) {
      if (ctx.reloading == 0) {
        elog(0, "wait child fail: wait(): %s", strerror(ERRNO));
      } else if (ctx.whether_reload == 0) {
        memcpy(workers, pool->workers, sz);
        ctx.whether_reload = 1;
        goto clean;
      }
    }

    if (*pool_status == 1) {
      for (i = 0; i < pool->num_workers; i++) {
        if (pid != pool->workers[i].pid) {
          continue;
        }
        int npid = spawn(pool, i);
        if (npid < 0) {
          elog(0, "refork process error: spawn(%x, %d)", pool, i)
        } else {
          pool->workers[i].pid = npid;
        }
      }
    }
clean:
    if (ctx.reloading == 1) {
      if (n >= pool->num_workers) {
        ctx.reloading = 0;
        n = 0;
        continue;
      }
      if (kill(workers[n].pid, SIGTERM) < 0) {
        continue;
      }
      n++;
    }
  }
  free(workers);
}

void process_pool_init(process_pool *pool, int num_workers) {
  assert(pool != NULL);

  int *ptr = mmap_malloc(64*1024);
  pool_status = ptr

  *pool_status = 1;
  ctx.process_type = MASTER_PROCESS;

  worker *workers;
  int n = num_workers * sizeof(worker);

  worker = malloc(n);
  if (worker == NULL) {
    elog(1, "init process pool: malloc(%d)", n, strerror(ERRNO));
  }

  pool->workers = workers;
  pool->num_workers = num_workers;
  pool->cur_wk = -1;
}

void process_pool_start(process_pool *pool) {
  int i,sz, pid;
  Pipe *pipes;

  process_pool *p = pool;
  sz = p->num_workers * sizeof(Pipe);

  pipes = malloc(sz);
  if (pipes == NULL) {
    elog(1, "pool start: malloc(%d): %s", sz, strerror(ERRNO));
  }

  for (i = 0; i < p->num_workers; i++) {
    socketpair(PF_LOCAL, SOCK_DGRAM, 0, pipes[i].pipe);
  }

  pid = fork();
  if (pid == -1) {
    elog(1, "start process pool: fork()", strerror(ERRNO));
  } else if (pid == 0) {
    for (i = 0; i < p->num_workers; i++) {
      close(pipes[i].pipe[0]);
      p->workers[i].pipe_fd = pipes[i].pipe[1];

      //管理进程创建工作进程
      pid = spawn(pool, i);

      if (pid < 0) {
        elog(0, "start process pool: spawn(%x, %d)", pool, i strerror(ERRNO));
        return;
      } else {
        p->workers[i].pid = pid;
      }
    }

    ctx.process_type = MANAGE_PROCESS;

    manager_process(p);
  } else {
    p->pid = pid;

    for (i = 0; i < p->num_workers; i++) {
      close(pipes[i].pipe[1]);
      p->workers[i].pipe_fd = pipes[i].pipe[0];
    }
  }
}

void process_pool_dispatch(process_pool *pool, void *data) {
  int n, m;
  job_t *job = (job_t*) data;

  n = job->fd % pool->num_workers;  
  m = write(pool->workers[n].pipe_fd, job, sizeof(job_t));

  if (m > 0) {
    emsg("master send job: buf=%s | fd=%d\n", job->buf, job->fd);
  }
}

void process_pool_end(process_pool *pool) {
}
