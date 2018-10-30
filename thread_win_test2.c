#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "lfq.h"

lfq_t myq;

static unsigned int __stdcall winthread_proc(PVOID param) {
	printf("%s\n", "running!!!!");
	void *data;
	while (1) {
		if ((data = lfq_deq(&myq)) == NULL) {
			SuspendThread(GetCurrentThread());
			fprintf(stderr, "%s\n", "Wake up!");
		}

		printf("%d\n", *((int*)data));
	}

	return 0;
}

int main(void) {

	HANDLE h;

	lfq_init(&myq);

	h = (HANDLE)_beginthreadex(0, 0, winthread_proc, NULL, CREATE_SUSPENDED, 0);

	int i, *data;
	for (i = 0; i < 1000; i++) {
		data = malloc(sizeof(int));
		assert(data != NULL);
		*data = i;
		lfq_enq(&myq, data);
	}
	ResumeThread(h);

	DWORD dw;
	dw = WaitForSingleObject(h, INFINITE);
	switch (dw) {
	case WAIT_FAILED:
		fprintf(stderr, "%s\n", "invalid handle??");
		break;
	default:
		fprintf(stderr, "%s\n", "Nothing!!!");
	}

	return 0;
}