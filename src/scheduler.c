#include <unistd.h>
#include <stdlib.h>
#ifndef SCHED_D
#define SCHED_D
#include "../include/queue.h"
#include "../include/global_var.h"
#endif


extern struct machine_t machine;
extern struct queue_t proc_queue;

/// @brief Scheduler FCFS
/// @return
void *scheduler()
{
    int i, j, z, ncpus, ncores, nthreads;
    struct pcb_t new_process;
    struct cpu_t *cpu = NULL;
    struct thread_t *threads_l = NULL;

    pthread_mutex_lock(&mutex_s);
    while (1)
    {
        sem_wait(&semaf_sched);
        ncpus = machine.n_cpu;
        printf("\n \n machine (scheduler) --%p-\n \n", &machine);
        printf("\n \ncpus (scheduler) --%d--\n \n", ncpus);
        ncores = machine.list_cpu[0].n_core;
        printf("\n \nncores (scheduler) --%d--\n \n", ncores);

        nthreads = machine.list_cpu[0].list_core[0].n_thread;

        cpu = machine.list_cpu;

        for (i = 0; i < ncpus; i++)
        {
            for (j = 0; j < ncores; j++)
            {
                threads_l = cpu[i].list_core[j].list_thr;

                for (z = 0; z < nthreads; z++)
                {
                    if (threads_l[z].pcb.fake_tll == 0)
                    {
                        new_process = dequeue(&proc_queue); // si no hay procesos en cola, aqui se devuelve idle_task
                        threads_l[z].pcb = new_process;
                        threads_l[z].pc = new_process.mm.code;
                    }
                }
            }
        }

        sem_post(&semaf_client1);
    }
}