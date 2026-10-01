/*
 * Implementacion de una cola FIFOs con una lista enlazada en C
 *
 */

#include "datatypes.h"

#ifndef QUEUE_H
#define QUEUE_H

// Nodo de lista enlazada, elemento de la cola de procesos
struct node_t
{
    struct pcb_t process;
    struct node_t *next;
};

// cola de procesos
struct queue_t
{
    struct node_t *front, *rear;
};

struct node_t *new_node(struct pcb_t new_process);

// Crea una cola vacia
struct queue_t createqueue();
// struct queue_t *createqueue();

// Encolar proceso
void enqueue(struct queue_t *q, struct pcb_t new_process);

// Desencolar proceso
struct pcb_t dequeue(struct queue_t *q);

#endif