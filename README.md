# Kernel Simulator (pthreads, C)

This is a **simple Operating System kernel simulator** written in **C with pthreads** for the Operative Systems course.

## Features
- Process Control Block (PCB) simplified  
- FIFO scheduler 
- Timer and context switch simulation  
- Synchronization with mutex and condition variables
- Simulated memory layout

## Project structure
- `include/` — header files (`.h`)
- `src/` — source files (`.c`)
- `prog/` — programs to load (`progXXX.elf`)

## How it works
The program simulates a multi-core machine with paged memory. The kernel components run as separate pthreads, all driven by a simulated clock.

### The simulated machine
- `num_cpus` CPUs × `cores_per_cpu` cores × `threads_per_core` hardware threads. Each hardware thread has a PC, an IR, a PTBR, 16 registers and an MMU with a 16-entry TLB.
- 64 MB of physical memory (2^24 words of 32 bits), divided into pages/frames of 4096 words:
  - the first 3584 frames hold the code and data of the programs;
  - the last 8 MB are reserved for page tables (up to 128 tables of 16384 entries).
- A bitmap keeps track of which memory words are in use.

### Threads
```
clock ──► timer_sched ──every <tick_sched> cycles──► scheduler
   └────► timer_ld    ──every <tick_pgen> cycles───► loader ──► ready queue
```
- **Clock** (`clock.c`): on each cycle it waits for both timers, advances the machine one cycle (`mover_maquina`) and wakes the timers again.
- **Timers** (`timer_sched.c`, `timer_ld.c`): count clock cycles. Every `tick_sched` / `tick_pgen` cycles they wake the scheduler / loader with a semaphore and wait until it finishes, so the clock is stopped while they work.
- **Loader** (`loader.c`): each time it is woken it loads the next program from `prog/`. It reserves a page table, finds free page table entries and frames, copies the program into physical memory, creates its PCB (pid, TTL, code/data addresses, page table address) and adds it to the ready queue. Once `num_programs` programs are loaded it does nothing else.
- **Scheduler** (`scheduler.c`): FCFS. Each time it is woken, every hardware thread whose process has finished gets the next process from the ready queue (or the idle task if the queue is empty).
- **Machine step** (`avanzar_maquina.c`): on each clock cycle, every hardware thread running a process translates the virtual address of its code into a physical one through the MMU (TLB first, then the page table), fetches that word into the IR, decrements the TTL of the process and increments the PC.

Other files: `queue.c` is the FIFO ready queue (a linked list). `process_gen.c` is an older random process generator; it is fully commented out and has been replaced by the loader.

### Program format (`prog/progXXX.elf`)
```
.text 000000    <- virtual address where the code starts
.data 000014    <- virtual address where the data starts
0C00002C        <- one 32-bit word per line, in hex: first the instructions, then the data
0D000030
2ECD0000
1E000034
F0000000
000000AC
...
```
The first hex digit of an instruction is the opcode and the second one is a register:

| Opcode | Instruction | Example    | Meaning           |
|--------|-------------|------------|-------------------|
| `0`    | `ld`        | `0C00002C` | `r12 ← [0x2C]`    |
| `1`    | `st`        | `1E000034` | `[0x34] ← r14`    |
| `2`    | `add`       | `2ECD0000` | `r14 ← r12 + r13` |
| `F`    | `exit`      | `F0000000` | end the process   |

### Output
The simulator prints the programs it loads and the scheduler activity to stdout, and one trace line per clock cycle to stderr. It keeps running until you stop it with Ctrl+C.

### Current limitations
- Instructions are fetched into the IR but not executed yet (`ld`, `st`, `add` and `exit` are not decoded at run time).
- A process ends when its simulated TTL (`fake_tll`) reaches 0, not when it reaches `exit`.
- The memory of finished processes is not freed.
- The loader starts at `prog001.elf` (`prog000.elf` is skipped), so with the 50 provided programs `num_programs` can be at most 49.

## Build
```bash
gcc -pthread -o kernel_simulator src/*.c
```

## Run
Run it from the project root, since the loader reads the programs from `prog/`:
```bash
./kernel_simulator <tick_sched> <tick_pgen> <num_cpus> <cores_per_cpu> <threads_per_core> <num_programs>
```

Example (scheduler and loader every 5 clock ticks, 1 CPU with 1 core and 1 thread, 3 programs):
```bash
./kernel_simulator 5 5 1 1 1 3
```

## Notes
- Educational use only  
