# Kernel Simulator (pthreads, C)

This is a **simple Operating System kernel simulator** written in **C with pthreads** for the Operative Systems course.

## Features
- Process Control Block (PCB) simplified  
- FIFO scheduler 
- Timer and context switch simulation  
- Synchronization with mutex and condition variables
- Simulated memory layout

## Build
```bash
gcc -pthread -o kernel_simulator *.c

## Notes
- Educational use only  
