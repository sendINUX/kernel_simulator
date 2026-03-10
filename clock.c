#include <unistd.h>
#include <stdlib.h>
#include "global_var.h"
#include "avanzar_maquina.h"

extern struct machine_t machine;


/// @brief Función que genera la señal de clock
/// @return
void *clock_fun()
{
    int temp_cant = 2; // cantidad de temporizadores

    while (1)
    {        
        pthread_mutex_lock(&mutex);
        

        while (done < (temp_cant))
        {
            pthread_cond_wait(&cond, &mutex); // esto desbloquea el mutex
        }
        done = 0;

        clock_pulse++;
        mover_maquina(&machine);
       
        // printf("Pulso del reloj: %d\n", clock_pulse);
        //fflush(stdout);
        pthread_cond_broadcast(&cond2);
        pthread_mutex_unlock(&mutex);
    }
}
