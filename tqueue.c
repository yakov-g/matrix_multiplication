#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#include "queue.h"
#include "tqueue.h"

struct _TQueue
{
   pthread_mutex_t queue_mutex;
   sem_t data_semaphore;
   Queue *queue;
};

void
tqueue_push(TQueue *tqueue, const void *data)
{
   if (!tqueue) return;
   if (!data) return;
   pthread_mutex_lock(&tqueue->queue_mutex);
   queue_push(tqueue->queue, data);
   pthread_mutex_unlock(&tqueue->queue_mutex);

   //sem_post(&tqueue->data_semaphore);
}

const void *
tqueue_get(TQueue *tqueue)
{
   const void *ret = NULL;
   /*clean up threads stuk on sem_wait */
   //sem_wait(&tqueue->data_semaphore);

   pthread_mutex_lock(&tqueue->queue_mutex);
   ret = queue_peek(tqueue->queue);
   if (ret)
     {
        queue_pop(tqueue->queue);
     }
   pthread_mutex_unlock(&tqueue->queue_mutex);
   return ret;
}

TQueue *
tqueue_create()
{
   TQueue *tqueue = (TQueue *) calloc(1, sizeof(TQueue));
   if (!tqueue) return NULL;
   tqueue->queue = queue_create();
   if (!tqueue->queue)
     {
        goto bad_end;
     }
   if (pthread_mutex_init(&tqueue->queue_mutex, NULL))
     {
        goto bad_end;
     }

   if (sem_init(&tqueue->data_semaphore, 0, 0) < 0)
     {
        goto bad_end;
     }

   return tqueue;

bad_end:
   queue_delete(tqueue->queue);
   free(tqueue);
   return NULL;
}

void
tqueue_delete(TQueue *tqueue)
{
   if (!tqueue) return;
   sem_destroy(&tqueue->data_semaphore);
   pthread_mutex_destroy(&tqueue->queue_mutex);
   queue_delete(tqueue->queue);
   free(tqueue);
}
