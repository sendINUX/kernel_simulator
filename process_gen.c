/* #include "global_var.h"
#include <stdio.h>
#include <stdlib.h>
#include "datatypes.h"
#include "queue.h"

#define MAX_TTL 1000 // TTL maximo en ciclos de reloj

void *process_gen(void *process_queue)
{
  int j, random_ttl;
  struct pcb_t proceso;
  j = 1;

  pthread_mutex_lock(&mutex_p);
  while (1)
  {
    random_ttl = (rand() % (MAX_TTL))+1;
    proceso.pid = j;
    proceso.fake_tll = random_ttl; 
    proceso.mm.pgb = 0;
    j++;

    sem_wait(&semaf_pgen);
    enqueue((struct queue_t*)process_queue, proceso);
    sem_post(&semaf_client2);
  }
}
 */