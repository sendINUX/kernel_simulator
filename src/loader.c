
#include <stdlib.h>
#include <stdio.h>
#ifndef LOADER
#define LODAER
#include "../include/queue.h"
#include "../include/global_var.h"
#endif

extern struct machine_t machine;
extern struct queue_t proc_queue;

#define MAX_TTL 1000 // TTL maximo en ciclos de reloj
// progXXX.elf. Donde XXX serán números consecutivos empezando por el 000.  (1000 programas) supongo que tiene que tomar de 0 hasta 999 ficheros o hasta que no haya?

// crear tabla de paginas y cargarla
int prog_i = 0;

/// @brief Devuelve instrucción segun valor hex.
/// @param instruction
/// @return
char *get_instruction(char instruction)
{
    char *instr;
    switch (instruction)
    {
    case '0':
        instr = "ld";
        break;
    case '1':
        instr = "st";
        break;
    case '2':
        instr = "add";
        break;
    case 'F':
        instr = "exit";
        break;
    default:
        fprintf(stderr, "Fallo al decodificar tipo de instruccion\n");
        exit(1);
    }
    return instr;
}

/// @brief Devuelve un registro según valor hexadecimal
/// @param resgistro en hexadecimal
/// @return resgistro
char *get_register(char resgistro)
{
    char *reg;
    switch (resgistro)
    {
    case '0':
        reg = "r0";
        break;
    case '1':
        reg = "r1";
        break;
    case '2':
        reg = "r2";
        break;
    case '3':
        reg = "r3";
        break;
    case '4':
        reg = "r4";
        break;
    case '5':
        reg = "r5";
        break;
    case '6':
        reg = "r6";
        break;
    case '7':
        reg = "r7";
        break;
    case '8':
        reg = "r8";
        break;
    case '9':
        reg = "r9";
        break;
    case 'A':
        reg = "r10";
        break;
    case 'B':
        reg = "r11";
        break;
    case 'C':
        reg = "r12";
        break;
    case 'D':
        reg = "r13";
        break;
    case 'E':
        reg = "r14";
        break;
    case 'F':
        reg = "r15";
        break;
    default:
        fprintf(stderr, "Fallo al decodificar registro de la instruccion\n");
        exit(1);
    }
    return reg;
}

/// @brief Comprueba si una posición de memoria está libre o ocupada
/// @param dir
/// @return 0 (false) ocupado o 1 (true) libre
int is_free(int32_t dir)
{
    return (!machine.bitmap_mem[dir]); // en el bit map 1 significa posicion ocupada y 0 significa libre
}

/// @brief Devuelve la direcion ptbr para la tabla de paginas
/// @return ptbr libre o -1 si no hay espacio para una tabla de paginas nueva
int32_t alocate_pt()
{
    for (int32_t free_ptbr = INICIO_RESERVADO_PT; free_ptbr < MEM_TAM; free_ptbr += PT_SIZE)
    {
        if (is_free(free_ptbr))
        {
            machine.bitmap_mem[free_ptbr] = 1; // marcar inicio de tabla
            machine.mem[free_ptbr] = -1;       // 0xFFFFFFFF, significa que sera el PTE 0 pero que todavia no tiene nada asignado
            return free_ptbr;
        }
    }
    return -1;
}



/// @brief  Busca en la PT si hay disponibles tantas PTEs como numero_de_pags
///
/// @param ptbr dir. fisica de Page Table Base Register
/// @param numero_de_pags Numero de paginas que solicitadas
/// @return NULL o puntero a cadena con PTEs disponibles
///
/// @attention Si no se devolvio NULL habra que hacer free
int32_t *get_free_ptes(int32_t ptbr, int numero_de_pags)
{
    int32_t *list_pte = malloc(numero_de_pags * sizeof(int32_t));
    int page, i;
    page = i = 0;

    if (is_free(ptbr) || machine.mem[ptbr] == -1)
    {
        list_pte[i] = page;
    }

    for (int32_t free_pte = ptbr + 1; free_pte < ptbr + PT_SIZE; free_pte++)
    {
        page++;
        if (is_free(free_pte))
        {
            i++;
            list_pte[i] = page;
        }

        if (i >= numero_de_pags)
            return list_pte;
    }

    free(list_pte);
    return NULL;
}


/// @brief Comprueba si hay suficientes frames disponibles en memoria como para almacena el programa
///
/// @param amount_pages Cantidad de paginas (frames) que usara el programa
/// @return  puntero avaliable_frames Frames libres o NULL si no habia
///
/// @attention NO se vacia available_frames aunque devuelva 1
int32_t *check_available_frames(int amount_pages)
{
    int32_t *avaliable_frames = malloc(amount_pages * sizeof(int32_t));
    int cont = 1;
    for (int j = 0; j < amount_pages; j++)
    {
        for (int32_t num_frame = 0; num_frame < NUM_FRAMES; num_frame += 1)
        {
            if (is_free(machine.bitmap_mem[num_frame * PAGE_SIZE]))
            {
                avaliable_frames[j] = num_frame; //
                cont++;
            }
        }
    }
    return ((cont < amount_pages) ? NULL : avaliable_frames);
}

/// @brief Escribe el programa en memoria fisica
/// @param filename
/// @param frame_list
/// @param num_frames
/// @param pcb
/// @return 0 si fue bien o interrupme la ejecucion
int store_program(int32_t *frame_list, int num_frames, struct pcb_t *pcb)
{
    FILE *fp;
    int32_t offset;
    int j;
    char filename[128];
    sprintf(filename, "prog/prog%03d.elf", prog_i);
    if ((fp = fopen(filename, "r")) == NULL)
    {
        fprintf(stderr, "Error al abrir el programa: %s\n", filename);
        exit(1);
    }

    if (fscanf(fp, ".text %d\n.data %d", &(pcb->mm.code), &(pcb->mm.data)) != 2)
    {
        fprintf(stderr, "Error al leer las primeras líneas (%s)\n", filename);
        fclose(fp);
        exit(1);
    }
    printf("\n EL PROGRAMA ES VALIDO: text = %d, data = %d\n", pcb->mm.code, pcb->mm.data);

    for (j = 0; j < num_frames; j++)
    {
        offset = 0;
        while (fscanf(fp, "%x", &(machine.mem[frame_list[j] + offset])) != EOF)
        {
            machine.bitmap_mem[frame_list[j] + offset] = 1;
            offset++;
            if (offset == PAGE_SIZE) // si ha escrito todo el frame salir
                break;
        }
    }
    fclose(fp);
    free(frame_list);
    return 0;
}

/// @brief Calcula cuantas paginas se necesitan para almacenar un programa en memoria
/// @param prog_size Se escribe el tamaño del programa
/// @return numero de paginas necesarias
///
/// @details El programa será el que indique el contador "prog_i"
int calculate_needed_pages()
{
    FILE *fp;
    int prog_size = 0;
    char linea[128];
    char filename[128];
    sprintf(filename, "prog/prog%03d.elf", prog_i);

    printf("\n\nPrograma a leer: (%s)", filename);
    if ((fp = fopen(filename, "r")) == NULL)
    {
        fprintf(stderr, "Fallo al abrir fichero programa (%s)\n", filename);
        exit(1);
    }

    // Comprueba el tamaño del fichero
    // while (EOF != (fscanf(fp, "%*[^\n]"), fscanf(fp, "%*c")))
    while (fgets(linea, 128, fp) != NULL)
        prog_size++;
    fclose(fp);

    return (((prog_size) % PAGE_SIZE) ? (((prog_size) / PAGE_SIZE) + 1) : (prog_size) / PAGE_SIZE); // cociente por exceso
}

/// @brief
/// @param ptbr_addr
/// @param filename
/// @param pcb
/// @return
int alocate_program(int32_t *ptbr_addr, struct pcb_t *pcb)
{
    int32_t *avaliable_frames, *pte_list;
    int pags_necesarias, i;
    int32_t num_frame, num_pag;

    if ((*ptbr_addr = alocate_pt()) == -1)
    {
        fprintf(stderr, "No hay espacio para reservar la tabla de paginas\n");
        return -1;
    }

    pags_necesarias = calculate_needed_pages();
    if ((pte_list = get_free_ptes(*ptbr_addr, pags_necesarias)) == NULL)
    {
        machine.bitmap_mem[*ptbr_addr] = 0;
        fprintf(stderr, "No hay suficientes PTEs disponibles en la tabla\n");
        return -1;
    }
    if ((avaliable_frames = check_available_frames(pags_necesarias)) == NULL)
    {
        machine.bitmap_mem[*ptbr_addr] = 0;
        fprintf(stderr, "No hay suficientes PTEs disponibles en la tabla\n");
        return -1;
    }

    for (i = 0; i < pags_necesarias; i++)
    {
        num_pag = pte_list[i];
        num_frame = avaliable_frames[i];
        machine.mem[*ptbr_addr + num_pag] = num_frame;
        machine.bitmap_mem[*ptbr_addr + num_pag] = 1;
        machine.bitmap_mem[num_frame * PAGE_SIZE] = 1;
    }
    pcb->mm.pgb = *ptbr_addr;
    store_program(avaliable_frames, pags_necesarias, pcb);
    return 0;
}

/// @brief Loader
/// @param Carga tantos programas como num_prog
/// @return
void *loader_fun(void *prog_num)
{
    struct pcb_t proceso;
    int pid, random_ttl;
    int32_t *ptbr_addr = malloc(sizeof(int32_t));

    pid = 1;
    random_ttl = (rand() % (MAX_TTL)) + 1;

    pthread_mutex_lock(&mutex_p);
    while (1)
    {
        if (prog_i < atoi(prog_num))
        {
            sem_wait(&semaf_ld);

            proceso.pid = pid;
            proceso.fake_tll = random_ttl;
            pid++;
            prog_i++;
            if (alocate_program(ptbr_addr, &proceso) == 0)
            {
                enqueue((struct queue_t *)&proc_queue, proceso);
            }

            sem_post(&semaf_client2); // si no hay tabla de paginas disponible no se puede cargar un proceso
        }
        else
        {
            fprintf(stderr, "Ya se cargaron la cantidad de programas seleccionados (%d)\n",atoi(prog_num));
            sem_wait(&semaf_ld);
            sem_post(&semaf_client2); // si no hay tabla de paginas disponible no se puede cargar un proceso
        }
    }
}

/*
void *loader_fun(void *prog_num)
{
    sem_wait(&semaf_ld);
    sem_post(&semaf_client2); // si no
} */



/*


ESTAS FUNCIONES ESTAN EN avanzar_maquina.c 


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


*/
