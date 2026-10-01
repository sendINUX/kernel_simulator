#include "datatypes.h"

#ifndef AVANZAR_MAQUINA_H
#define AVANZAR_MAQUINA_H

void mover_maquina(struct machine_t *maquina);
// int initialize_machine(struct machine_t **maquina, int cpus, int cores, int threads);
int free_machine(struct machine_t *maquina);
//int get_num_threads(struct machine_t *maq);

//struct thread_t **get_threads(struct machine_t *maquina, int *num_threads);
#endif