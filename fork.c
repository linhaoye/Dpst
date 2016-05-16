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

/* 全局标识符 */
struct {
	char status;
	uint8_t process_type;
	int manager_worker_reloading;
	int manager_reload_flag;
} SG;

/* 进程标识符 */
enum {
	MASTER_PROCESS,
	WORKER_PROCESS,
	MANAGE_PROCESS
};

typedef struct {
	int pipe[2];
} Pipe;

typedef struct {
	char buf[512];
	Socket fd;
} Jobs;

typedef struct {
	pid_t pid;
	int pipe_fd;
} Worker;

typedef struct {
	Worker *workers;
	int pid;
	int num_workers;
	int cur_wk;
} DSPT;

void Worker_main(DSPT *dspt, int p) {
	int nRead;
	pid_t pid;
	Jobs jobs;

	pid = getpid();
	while (1) {
		nRead = read(p, &jobs, sizeof(Jobs));
		if (nRead > 0) {
			printf("Worker[%d] recv jobs: buf=%s | fd=%d\n", pid, jobs.buf, jobs.fd);
		}
	}
}

int Worker_exec(DSPT *dspt, int n) {
	int i, pid;	
	DSPT *dsptCTX = dspt;

	pid = fork();
	if (pid < 0) {
		perror("fork");
		return -1;
	} else if (pid == 0) { //work进程
		for (i = 0; i < dsptCTX->num_workers; i++) {
			//关闭子进程非当前的管道
			if (n != i)
				close(dsptCTX->workers[i].pipe_fd);
		}
		//进程标识
		SG.process_type = WORKER_PROCESS;
		//进程处理过程
		Worker_main(dsptCTX, dsptCTX->workers[n].pipe_fd);
		exit(0);
	} else {
		return pid;
	}
}

void SIG_sigusr1(int signo) {
	SG.manager_worker_reloading = 1;
	SG.manager_reload_flag = 0;
}

void SIG_sigterm(int signo) {
	SG.status = 0;
}

/* 管理进程过程 */
void DSPT_main(DSPT *dspt) {
	int i;
	int n;
	pid_t pid;
	Worker rworkers;

	(void) signal(SIGUSR1, SIG_sigusr1);
	(void) signal(SIGTERM, SIG_sigterm);

	rworkers = Realloc(NULL, dspt->num_workers * sizeof(Worker));

	while (SG.status) {
		pid = wait(NULL);

		if (pid < 0) {
			if (SG.manager_worker_reloading == 0) {
				perror("DSPT_main wait");
			} else if (SG.manager_reload_flag == 0) {
				memcpy(rworkers, dspt->workers, sizeof(Worker) * dspt->num_workers);
				SG.manager_reload_flag = 1;
				goto CLEAN;
			}
		}
		//某个 worker进程意外结束
		if (SG.status && pid != -1) {
			for (i = 0; i < dspt->num_workers; i++) {
				if (pid != dspt->workers[i].pid)
					continue;
				//重新起一个worker进程
				int npid = Worker_exec(dspt, i);
				if (npid < 0) {
					perror("DSPT_main refork error");
					return;
				} else {
					dspt->workers[i].pid = npid;
				}
			}
		}
		CLEAN:
		if (SG.manager_worker_reloading == 1) {
			if (n >= dspt->num_workers) {
				SG.manager_worker_reloading = 0;
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

void DSPT_init(DSPT *dspt, int num_workers) {
	assert(dspt != NULL);
	Worker *workers;
	workers = Realloc(NULL, num_workers * sizeof(Worker));

	dspt->workers = workers;
	dspt->num_workers = num_workers;
	dspt->cur_wk = -1;
}

void DSPT_start(DSPT *dspt) {
	int i, pid;
	Pipe *pipes;
	DSPT *dsptCTX = dspt;
	pipes = Realloc(NULL, dsptCTX->num_workers * sizeof (Pipe));

	//管道设为全双工
	for (i = 0; i < dsptCTX->num_workers; i++) {
		socketpair(PF_LOCAL, SOCK_DGRAM, 0, pipes[i].pipe);
	}
	pid = fork();
	if (pid == -1) {
		perror("fork");
		exit(1);
	}

	//创建管理进程
	if (pid == 0) {
		for (i = 0; i < dsptCTX->num_workers; i++) {
			//管理进程的管道只开启写端
			close(pipes[i].pipe[0]);
			dsptCTX->workers[i].pipe_fd = pipes[i].pipe[1];
			//创建Worker进程
			pid = Worker_exec(dspt, i);
			if (pid < 0) {
				perror("Worker_exec!");
				return;
			} else {
				dsptCTX->workers[i].pid = pid;
			}
		}

		//标识为管理进程
		SG.process_type = MANAGE_PROCESS;
		DSPT_main(dsptCTX);
	} else { //主进程
		dsptCTX->pid = pid;
		//主进程管道只开启读端
		for (i = 0; i < dsptCTX->num_workers; i++) {
			close(pipes[i].pipe[1]);
			dsptCTX->workers[i].pipe_fd = pipes[i].pipe[0];
		}
	}
}

void DSPT_dispatch(DSPT *dspt, void *data) {
	int n, nWrite;
	Jobs *jobs = (Jobs*) data;

	//使用fd取模散列
	n = jobs->fd % dspt->num_workers;	
	nWrite = write(dspt->workers[n].pipe_fd, jobs, sizeof(Jobs));

	if (nWrite > 0) {
		printf ("Master send jobs: buf=%s | fd=%d\n", jobs->buf, jobs->fd);
	}
}

#ifdef __MAIN__
int main(void)
{
	int i, num_jobs;
	DSPT dspt;
	Vec(Jobs) cq;
	vec_init(&cq);

	num_jobs = 10;
	for (i = 0; i < num_jobs; i++) {
		Jobs jobs = {"Yhm, Forever!", i};
		vec_push(&cq, jobs);
	}

	printf("Master[%d] Runnig\n", getpid());
	SG.status = 1;
	SG.process_type = MASTER_PROCESS;

	DSPT_init(&dspt, 3);
	DSPT_start(&dspt);

	for (i= 0; i < num_jobs; i++) {
		DSPT_dispatch(&dspt, &cq.data[i]);
	}

	wait(NULL);

	vec_deinit(&cq);

	return 0;
}
#endif