#ifndef __THREAD_WIN__
#define __THREAD_WIN__

#include <windows.h>

typedef struct {
	int busy;
	int exit;
	HANDEL thread;
	PVOID param;
} thread;


#endif
