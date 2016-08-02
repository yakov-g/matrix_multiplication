/* Queue data structure implementation. */

#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct _Queue Queue;

/* Push data to queue. NULL data allowed. */
void
queue_push(Queue *queue, const void *data);

/* Pop data from queue. */
void
queue_pop(Queue *queue);

/* Take top data from queue. */
const void *
queue_peek(const Queue *queue);

/* Create queue. */
Queue *
queue_create();

/* Destroy queue. */
void
queue_destroy(Queue *queue);

#endif
