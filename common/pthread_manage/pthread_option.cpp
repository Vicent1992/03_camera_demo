#include "pthread_option.h"


/************ use guide
const static int thread_num = 10;
int thread_idx[thread_num];
pthread_t tid[thread_num];

for (int i = 0; i < thread_num; i++) {
	thread_idx[i] = i;
	if (StartThread(tid[i], jpeg_encode_routine, (void*)(thread_idx + i))) {
		printf("start thread <%d> failed\n", i);
		break;
	}
}
 
for (int i = 0; i < thread_num; i++) {
	if (tid[i])
		StopThread(tid[i]);
}
 ************/

int StartThread(pthread_t &tid, void* (*Threadloop)(void*), void* arg) 
{
	int ret = 0;
	int stacksize = 64 * 1024;
	pthread_attr_t attr;

	if (pthread_attr_init(&attr)) {
		printf("pthread_attr_init failed!\n");
		return -1;
	}

	if (pthread_attr_setstacksize(&attr, stacksize)) {
		printf("pthread_attr_setstacksize <%d> failed!\n", stacksize);
		ret = -1;
		goto out;
	}

	if (pthread_create(&tid, NULL, Threadloop, arg)) {
		printf("pthread create failed!\n");
		ret = -1;
	}
	
out:
	if (pthread_attr_destroy(&attr))
		printf("pthread_attr_destroy failed!\n");
	return ret;
}

void StopThread(pthread_t &tid) 
{
	if (tid) {
		pthread_join(tid, nullptr);
		tid = 0;
	}
}

