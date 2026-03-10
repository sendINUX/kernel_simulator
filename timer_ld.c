#include "global_var.h"
#include <stdlib.h>

void *timer_ld(void *tick_loader)
{
  int cont;
  cont = 0;

  pthread_mutex_lock(&mutex);

  while (1)
  {
    done++;

    if (cont < atoi(tick_loader)-1)
    {
      cont++;
    }
    else
    { // esto se llamara cada tick_loader veces
      cont = 0;
    /*  printf("LLAMANDO pgen clock=%d", clock_pulse);
      fflush(stdout);         */   
      
      sem_post(&semaf_ld);
      sem_wait(&semaf_client2); 
    }
    pthread_cond_signal(&cond); // Manda señal tick
    pthread_cond_wait(&cond2, &mutex);
  }
}