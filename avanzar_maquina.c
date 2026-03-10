#include <stdlib.h>
#include <pthread.h>
#include "datatypes.h"
#include "avanzar_maquina.h"
#include "header.h"
#include "global_var.h"
#include <stdbool.h>

extern struct machine_t machine;



int free_machine(struct machine_t *maquina)
{
    int i, j;
    if (maquina == NULL)
    {
        return 1; // Evitar desreferenciación de puntero nulo
    }

    for (i = 0; i < (maquina->n_cpu); i++)
    {
        for (j = 0; j < (maquina->list_cpu[i].n_core); j++)
        {
            free(maquina->list_cpu[i].list_core[j].list_thr);
        }
        free(maquina->list_cpu[i].list_core);
    }
    free(maquina->list_cpu);

    maquina->list_cpu = NULL;
    maquina->n_cpu = 0;
    return 0;
}

/// @brief Obtiene ptes libres (en teoria siempre estaran libres pero bueno)
/// @param pag_num
/// @param thread
/// @return
int32_t get_frame_from_tlb(int32_t pag_num, struct thread_t *thread)
{
    int32_t tlb_entry, pag_n, frame_n;
    int i;
    for (i = 0; i < TLB_TAM; i++)
    {
        tlb_entry = thread->mmu.tlb[i];
        pag_n = tlb_entry >> 12;               // elimina los bits del frame
        pag_n = pag_n & ((1 << BITS_PAG) - 1); // lee pag
        if (pag_n == pag_num)
        {
            frame_n = tlb_entry & ((1 << 12) - 1); // lee frame
            return (frame_n);
        }
    }
    return -1;
}

/// @brief Dada un direccion virtual la convierte en fisica
///
/// @param dir_virtual
/// @param ptbr
/// @param offset_modify Si
/// @return direccion fisica
int32_t mmu_virtual_to_physical(int32_t dir_virtual, int32_t ptbr, bool offset_modify, struct thread_t *thread)
{
    int32_t pte_addr, dir_fisica, n_frame, n_pag, offset;

    n_pag = dir_virtual >> 12;                       // elimina offset
    n_pag = n_pag & ((1 << BITS_PAG) - 1);           // lee el numero de pagina
    offset = dir_virtual & ((1 << BITS_OFFSET) - 1); // lee el valor de offset
    if (offset_modify)
        offset = offset / 4;

    // Si la pagina no esta en TLB, obtener frame de memoria
    if ((n_frame = get_frame_from_tlb(n_pag, thread)) == -1)
    {
        pte_addr = ptbr + n_pag;
        n_frame = machine.mem[pte_addr];
    }

    // Calcula dir fisica
    dir_fisica = n_frame * PAGE_SIZE + offset;
    return (dir_fisica); // Devuelve direccion fisica
};


// avanzar maquina por cada clock
void mover_maquina(struct machine_t *maquina)
{
    int i, j, z, ncpus, ncores, nthreads;
    struct cpu_t *cpu;
    struct thread_t *thread;

    ncpus = maquina->n_cpu;
    ncores = maquina->list_cpu[0].n_core;
    nthreads = maquina->list_cpu[0].list_core[0].n_thread;

    cpu = maquina->list_cpu;
    fprintf(stderr, "moviendo maquina\n list_cpu es:  %p--\n", cpu);

    for (i = 0; i < ncpus; i++)
    {
        for (j = 0; j < ncores; j++)
        {
            thread = cpu[i].list_core[j].list_thr;
            for (z = 0; z < nthreads; z++)
            {
                if (thread[z].pcb.fake_tll > 0)
                {
                    int32_t dir_fisica = mmu_virtual_to_physical(thread[z].pcb.mm.code, thread[z].pcb.mm.pgb, (bool)1, &(thread[z]));
                    thread[z].ir = machine.mem[dir_fisica];
                    // ejecutar aqui IR
                    thread[z].pcb.fake_tll--;
                    thread[z].pc++;
                }
            }
        }
    }
}