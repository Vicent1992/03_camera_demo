#ifndef __PTHREAD_OPTION_H__
#define __PTHREAD_OPTION_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>



int StartThread(pthread_t &tid, void* (*Threadloop)(void*), void* arg);
void StopThread(pthread_t &tid);


#endif

