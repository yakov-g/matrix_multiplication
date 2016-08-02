#ifndef _T_QUEUE_H_
#define _T_QUEUE_H_

/* pthread safe queue. */

typedef struct _TQueue TQueue;

/* Create tqueue. */
TQueue *
tqueue_create();

/* Destroy tqueue. */
void
tqueue_destroy(TQueue *queue);

/* Push data to tqueue. NULL allowed. */
void
tqueue_push(TQueue *tqueue, const void *data);

/* Get data from tqueue. */
const void *
tqueue_get(TQueue *tqueue);

#endif
