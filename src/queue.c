/*
 * Implementacion de una cola FIFOs con una lista enlazada en C
 *
 */
#include "../include/datatypes.h"
#include "../include/queue.h"
#include <stdlib.h>

struct pcb_t idle_task = {0, 0,{0,0,0}};

struct node_t *new_node(struct pcb_t new_process)
{
    struct node_t *temp = (struct node_t *)malloc(sizeof(struct node_t));
    temp->process = new_process;
    temp->next = NULL;
    return temp;
}

/*
// crea cola nueva
struct queue_t* createqueue()
{
   struct queue_t* q = (struct queue_t*)malloc(sizeof(struct queue_t));
   q->front = q->rear = NULL;
   return q;
} */

// crea cola nueva
struct queue_t createqueue()
{
    struct queue_t q;
    q.front = q.rear = NULL;
    return q;
}

// Funcion para encolar
void enqueue(struct queue_t *q, struct pcb_t new_process)
{
    struct node_t *temp = new_node(new_process);
    if (q->rear == NULL)
    {
        q->front = q->rear = temp;
        return;
    }
    // anadir nodo y actualizar final de cola
    q->rear->next = temp;
    q->rear = temp;
}

// Funcion para eliminar primer elemento de la cola
struct pcb_t dequeue(struct queue_t *q)
{
    if (q->front == NULL)
        return idle_task; // si no hay procesos en cola devuelve el "swapper" o proceso idle

    // Store previous front and move front one node ahead
    struct node_t *temp = q->front;

    q->front = q->front->next;

    // If front becomes NULL, then change rear also as NULL
    if (q->front == NULL)
        q->rear = NULL;

    return temp->process;
    free(temp);
}

int esta_vacia(struct queue_t *q)
{
    if (q->front == NULL && q->rear == NULL)
        return 1;
    return 0;
}

// eliminar cola
void removequeue(struct queue_t *q)
{
    while (dequeue(q).pid)
        ;
}
