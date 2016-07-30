#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct _Queue Queue;

void
queue_push(Queue *queue, const void *data);

void
queue_pop(Queue *queue);

const void *
queue_peek(const Queue *queue);

Queue *
queue_create();

void
queue_destroy(Queue *queue);

#endif
