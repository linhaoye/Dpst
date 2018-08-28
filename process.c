#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>

/**
 * global process mamanger object
 */
static struct {
	char status;  // status of process pool
	uint8_t type; //process type
	int wstat; //
	int mstat;
} pTag;


/* process tag */
enum {
	MASTER_PROCESS,
	WORKER_PROCESS,
	MANAGE_PROCESS
};

/* process status */
enum {
	RUN,
	RELOAD
};

typedef struct {
	int pipe[2];
} Pipe;

typedef struct {
	char buf[512];
	int fd;
} Jobs;

typedef struct {
	pid_t pid;
	int pipe_fd;
} worker;

typedef struct {
	worker *workers;
	int pid;
	int num_workers;
	int cur_wk;
} process_pool;

static int spawn_worker(process_pool *pool, int n) {
	int i, pid;	
	process_pool *ctx = pool;

	pid = fork();
	if (pid < 0) {
		elog(0,"spawn worker: fork(): %s", strerror(ERRNO));
		return -1;
	} else if (pid == 0) {
		for (i = 0; i < ctx->num_workers; i++) {
			if (n != i) {
				close(ctx->workers[i].pipe_fd);
			}
		}
		pTag.type = WORKER_PROCESS;
		worker_process(ctx, ctx->workers[n].pipe_fd);
		return 0;
	} else {
		return pid;
	}
}

static void worker_process(process_pool *pool, int p) {
	int nRead;
	pid_t pid;
	Jobs jobs;

	pid = getpid();
	while (1) {
		nRead = read(p, &jobs, sizeof(Jobs));
		if (nRead > 0) {
			printf("worker[%d] recv jobs: buf=%s | fd=%d\n", pid, jobs.buf, jobs.fd);
		}
	}
}

static void manager_process(process_pool *pool) {
	int i;
	int n;
	pid_t pid;
	worker rworkers;

	(void) signal(SIGUSR1, sigusr1);
	(void) signal(SIGTERM, sigterm);

	rworkers = Realloc(NULL, pool->num_workers * sizeof(worker));

	while (pTag.status) {
		pid = wait(NULL);

		if (pid < 0) {
			if (pTag.wstat == 0) {
				perror("manager_process wait");
			} else if (pTag.mstat == 0) {
				memcpy(rworkers, pool->workers, sizeof(worker) * pool->num_workers);
				pTag.mstat = 1;
				goto CLEAN;
			}
		}
		//某个 worker进程意外结束
		if (pTag.status && pid != -1) {
			for (i = 0; i < pool->num_workers; i++) {
				if (pid != pool->workers[i].pid)
					continue;
				//重新起一个worker进程
				int npid = spawn_worker(pool, i);
				if (npid < 0) {
					perror("manager_process refork error");
					return;
				} else {
					pool->workers[i].pid = npid;
				}
			}
		}
		CLEAN:
		if (pTag.wstat == 1) {
			if (n >= pool->num_workers) {
				pTag.wstat = 0;
				n = 0;
				continue;
			}
			if (kill(rworkers[n].pid, SIGTERM) < 0) {
				continue;
			}
			n ++;
		}
	}
	
	Free(rworkers);
}

void sigusr1(int signo) {
	pTag.wstat = 1;
	pTag.mstat = 0;
}

void sigterm(int signo) {
	pTag.status = 0;
}

void process_pool_init(process_pool *pool, int num_workers) {
	assert(pool != NULL);

	pTag.status = 1;
	pTag.type = MASTER_PROCESS;

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
	int i, pid;
	Pipe *pipes;
	process_pool *ctx = pool;
	pipes = Realloc(NULL, ctx->num_workers * sizeof (Pipe));

	for (i = 0; i < ctx->num_workers; i++) {
		socketpair(PF_LOCAL, SOCK_DGRAM, 0, pipes[i].pipe);
	}

	pid = fork();
	if (pid == -1) {
		elog(1, "start process pool: fork()", strerror(ERRNO));
	}

	if (pid == 0) {
		for (i = 0; i < ctx->num_workers; i++) {
			close(pipes[i].pipe[0]);
			ctx->workers[i].pipe_fd = pipes[i].pipe[1];
			pid = spawn_worker(pool, i);
			if (pid < 0) {
				elog(0, "start process pool: spawn_worker(%x, %d)", 
					pool, i strerror(ERRNO));
				return;
			} else {
				ctx->workers[i].pid = pid;
			}
		}
		pTag.type = MANAGE_PROCESS;
		manager_process(ctx);
	} else {
		ctx->pid = pid;
		for (i = 0; i < ctx->num_workers; i++) {
			close(pipes[i].pipe[1]);
			ctx->workers[i].pipe_fd = pipes[i].pipe[0];
		}
	}
}

void process_pool_dispatch(process_pool *pool, void *data) {
	int n, nWrite;
	Jobs *jobs = (Jobs*) data;

	n = jobs->fd % pool->num_workers;	
	nWrite = write(pool->workers[n].pipe_fd, jobs, sizeof(Jobs));

	if (nWrite > 0) {
		printf ("Master send jobs: buf=%s | fd=%d\n", jobs->buf, jobs->fd);
	}
}


#ifdef __MAIN__
int main(void)
{
	int i, num_jobs;
	process_pool pool;
	Vec(Jobs) cq;
	vec_init(&cq);

	num_jobs = 10;
	for (i = 0; i < num_jobs; i++) {
		Jobs jobs = {"Yhm, Forever!", i};
		vec_push(&cq, jobs);
	}

	printf("Master[%d] Runnig\n", getpid());


	process_pool_init(&pool, 3);
	process_pool_start(&pool);

	for (i= 0; i < num_jobs; i++) {
		process_pool_dispatch(&pool, &cq.data[i]);
	}

	wait(NULL);

	vec_deinit(&cq);

	return 0;
}
#endif