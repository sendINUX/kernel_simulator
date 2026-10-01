#include "../include/queue.h"
#include "../include/header.h"
#include "../include/global_var.h"
#include <stdlib.h>
#include "../include/avanzar_maquina.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_s = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_p = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;  // clk
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER; // tick
pthread_cond_t sched = PTHREAD_COND_INITIALIZER;
pthread_cond_t loader = PTHREAD_COND_INITIALIZER;

int done = 0;
int clock_pulse = 0;

sem_t semaf_sched;
sem_t semaf_ld;
sem_t semaf_client1;
sem_t semaf_client2;

struct queue_t proc_queue;
struct machine_t machine;

int initialize_machine(int cpus, int cores, int threads)
{
    int i, j, z;
    struct pcb_t null_process = {-1, 0, {0, 0, 0}}; // pid -1, ttl 0

    printf("direccion de machine al inicializar %p\n", &machine);
    machine.list_cpu = (struct cpu_t *)malloc(cpus * sizeof(struct cpu_t));
    machine.n_cpu = cpus;
    printf("\n \n ncpu cpu %d\n", machine.n_cpu);

    for (i = 0; i < cpus; i++)
    {
        //  machine.list_cpu[i].list_core = malloc(cores * sizeof(struct core_t));
        machine.list_cpu[i].list_core = (struct core_t *)malloc(cores * sizeof(struct core_t));

        machine.list_cpu[i].n_core = cores;
        for (j = 0; j < cores; j++)
        {
            // machine.list_cpu[i].list_core[j].list_thr = malloc(threads * sizeof(struct thread_t));
            machine.list_cpu[i].list_core[j].list_thr = (struct thread_t *)malloc(threads * sizeof(struct thread_t));
            machine.list_cpu[i].list_core[j].n_thread = threads;

            for (z = 0; z < threads; z++)
            {
                machine.list_cpu[i].list_core[j].list_thr[z].pcb = null_process;
                machine.list_cpu[i].list_core[j].list_thr[z].mmu.tlb = malloc(TLB_TAM * sizeof(int32_t));
                for (int k = 0; k < TLB_TAM; k++)
                {
                    machine.list_cpu[i].list_core[j].list_thr[z].mmu.tlb[k] = -1; // los bits de informacion de control estan a 1 ahora
                }
                machine.list_cpu[i].list_core[j].list_thr[z].ir = 0;
                machine.list_cpu[i].list_core[j].list_thr[z].pc = 0;
                machine.list_cpu[i].list_core[j].list_thr[z].ptbr = 0;
            }
        }
    }
    machine.mem = calloc(MEM_TAM, sizeof(int32_t));
    machine.bitmap_mem = calloc(MEM_TAM, sizeof(bool));
    return 0;
}

int main(int argc, char *argv[])
{

    // tick_sched (numero de ciclos de reloj a esperar por cada tick que despierta a scheduler)
    // tick_pgen (numero de ciclos de reloj a esperar por cada tick que despierta a process generator)
    // numero de cpu de la maquina
    // numero de cores por cpu  (suponemos que las cpus son iguales)
    // numero de hilos por cada core
    // numero de programas a simular (a cargar)
    if (argc != 7)
    {
        fprintf(stderr, "Uso: %s <tick_sched> <tick_pgen>  <numero_de_cpus> <cores-por-cada-cpu> <hilos-por-cada-core> <num_de_programas_a_simular>\n", argv[0]);
        exit(1);
    }

    pthread_t clock_thr;
    pthread_t timer_pg_thr;
    pthread_t timer_sched_thr;
    pthread_t scheduler_thr;
    pthread_t loader_thr;

    proc_queue = createqueue();

    initialize_machine(atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
    printf("n_cpus desde main --%d--", machine.n_cpu);
    printf("n_cores desde main --%d--", machine.list_cpu[0].n_core);

    sem_init(&semaf_client1, 0, 0); // semaforos no nombrados
    sem_init(&semaf_client2, 0, 0); // valor a 0
    sem_init(&semaf_sched, 0, 0);   // semaforos no nombrados
    sem_init(&semaf_ld, 0, 0);      // valor a 0

    // Crear los threads de clock, process_gen, timer y scheduler
    pthread_create(&timer_pg_thr, NULL, timer_ld, (void *)argv[2]);
    pthread_create(&timer_sched_thr, NULL, timer_sched, (void *)argv[1]);
    pthread_create(&clock_thr, NULL, clock_fun, NULL);
    pthread_create(&scheduler_thr, NULL, scheduler, NULL);
    pthread_create(&loader_thr, NULL, loader_fun, (void *)argv[6]);

    // Crear los threads de los procesos
    pthread_join(clock_thr, NULL);
    pthread_join(timer_pg_thr, NULL);
    pthread_join(timer_sched_thr, NULL);
    pthread_join(scheduler_thr, NULL);
    pthread_join(loader_thr, NULL);
    free_machine(&machine);

    sem_destroy(&semaf_sched);
    sem_destroy(&semaf_ld);
    sem_destroy(&semaf_client1);
    sem_destroy(&semaf_client2);

    exit(0);
}
