#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#include "queue.h"
#include "tqueue.h"

struct _TQueue
{
   size_t wait_count;
   int close;
   pthread_mutex_t queue_mutex;
   sem_t data_semaphore;
   Queue *queue;
};

void
tqueue_push(TQueue *tqueue, const void *data)
{
   if (!tqueue) return;
   pthread_mutex_lock(&tqueue->queue_mutex);
   queue_push(tqueue->queue, data);
   pthread_mutex_unlock(&tqueue->queue_mutex);

   sem_post(&tqueue->data_semaphore);
}

const void *
tqueue_get(TQueue *tqueue)
{
   const void *ret = NULL;
   /*clean up threads stuk on sem_wait */

   pthread_mutex_lock(&tqueue->queue_mutex);
   tqueue->wait_count++;
   pthread_mutex_unlock(&tqueue->queue_mutex);

   sem_wait(&tqueue->data_semaphore);
   if (tqueue->close) return NULL;

   pthread_mutex_lock(&tqueue->queue_mutex);
   tqueue->wait_count--;

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
   queue_destroy(tqueue->queue);
   free(tqueue);
   return NULL;
}

void
tqueue_destroy(TQueue *tqueue)
{
   if (!tqueue) return;
   tqueue->close = 1;
   size_t i = 0;
   for(i = 0; i < tqueue->wait_count; i++)
      sem_post(&tqueue->data_semaphore);

   sem_destroy(&tqueue->data_semaphore);
   pthread_mutex_destroy(&tqueue->queue_mutex);
   queue_destroy(tqueue->queue);
   free(tqueue);
}
