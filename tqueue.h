#ifndef _T_QUEUE_H_
#define _T_QUEUE_H_

typedef struct _TQueue TQueue;

void
tqueue_push(TQueue *tqueue, const void *data);

const void *
tqueue_get(TQueue *tqueue);

TQueue *
tqueue_create();

void
tqueue_delete(TQueue *queue);

#endif
