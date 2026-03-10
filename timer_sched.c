#include "global_var.h"
#include <stdlib.h>

void *timer_sched(void *tick_sched)
{
  int cont;
  cont = 0;

  pthread_mutex_lock(&mutex);

  while (1)
  {
    done++;

    if (cont < atoi(tick_sched)-1)
    {
      cont++;
    }
    else
    { // esto se llamara cada tick_sched veces
      cont = 0;
    /*  printf("LLAMANDO SCHEDULER clock=%d", clock_pulse);   fflush(stdout); */
      // llamada a scheduler;
      sem_post(&semaf_sched);
      sem_wait(&semaf_client1); 
    }
    pthread_cond_signal(&cond); // Manda señal tick
    pthread_cond_wait(&cond2, &mutex);
  }
}