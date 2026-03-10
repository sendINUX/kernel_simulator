#include <stdint.h>
#include <stdbool.h>

#ifndef DATATYPES_H
#define DATATYPES_H

#define MEM_TAM 16777216 // Memoria: 2^24 direcciones * (4 B palabra) = 64 MB

// Usable 54MB, caben   --->  14680064/2^12 = 3584 paginas en memoria (RAM)

#define INICIO_RESERVADO_PT 14680064 // Espacio para 128 Page Tables (2^14 PTEs cada tabla) = 8 MB
#define NUM_FRAMES 3584              // Numero total de frames en memoria fisica
#define PAGE_SIZE 4096               // 16 KB (4096 words)
#define PTE_SIZE 1                   // 4B    (1 word)
#define PT_SIZE 16384                // 64KB  (2^14 ptes de 1 word cada)

#define VIRTUAL_ADDR_SIZE 24  // tamaino en bits
#define PHYSICAL_ADDR_SIZE 26 // bits

#define BITS_OFFSET 12 // bits de la direccion que indica el offset
#define BITS_PAG 14    // bits de la direccion virtual que indica la pagina

#define TLB_TAM 16



// tiene que ir desde 0 hasta el limite de INICIO_RESERVA y desde INICIO_RESERVA a  <= (menor igual)MEM_TAM-1

// mm (code, data y pgb)
struct mm_t
{
  int32_t code;    // Dirección virtual del inicio del segmento de código.
  int32_t data;    // Dir. virtual del inicio del segmento de datos.
  int32_t pgb;     // Dir. física de su tabla de páginas.
};

// memory management unit
struct mmu_t
{
  int32_t *tlb; // tlb
};

/// @brief Pcb
struct pcb_t
{
  int pid;
  int fake_tll; // tiempo que estara activo en proceso
  struct mm_t mm;
};

struct thread_t
{
  struct pcb_t pcb;
  int32_t ptbr;     // puntero a page table base register
  int32_t ir;       // instruction register
  int32_t pc;       // program counter
  struct mmu_t mmu;
  int32_t reg[16]; 
};

struct core_t
{
  int n_thread;
  struct thread_t *list_thr;
};

struct cpu_t
{
  int n_core;
  struct core_t *list_core;
};

struct machine_t
{
  int n_cpu;
  struct cpu_t *list_cpu;
  int32_t *mem;
  bool *bitmap_mem;
};

#endif // DATATYPES_H
