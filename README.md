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
