#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

#ifndef GLOBAL_VAR_H
#define GLOBAL_VAR_H

extern pthread_mutex_t mutex;
extern pthread_mutex_t mutex_s;
extern pthread_mutex_t mutex_p;
extern pthread_cond_t cond;
extern pthread_cond_t cond2;

extern sem_t semaf_sched;
extern sem_t semaf_ld;
extern sem_t semaf_client1;
extern sem_t semaf_client2;

extern int clock_pulse;
extern int done;

//extern struct machine_t machine;

#endif // GLOBAL_VAR_H