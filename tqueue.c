#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#include "queue.h"
#include "tqueue.h"

/* pthread safe queue implementation.
 * Based on Queue.
 * Provides thread safe push and get of data from queue. */

/* TQueue handler */
struct _TQueue
{
   Queue *queue; /* Queue itself*/

   size_t wait_count; /* Counter of threads which currently waiting. */
   int close; /* flag to signal that tqueue must be closed. */
   pthread_mutex_t queue_mutex; /* mutex to lock access to tqueue. */
   sem_t data_semaphore; /* Semaphore to signal that task was pushed. */
};

/* Create tqueue.
 * Creates handler and initializes variables. */
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

/* Destroy tqueue.
 *
 * When called, safely destroys queue.
 **/
void
tqueue_destroy(TQueue *tqueue)
{
   if (!tqueue) return;
   /* Rise a flag that queue is going to be closed */
   tqueue->close = 1;
   size_t i = 0;

   /* Increment semaphore in order to release,
    * threads which are currently waiting on sem_wait() in tqueue_get(). */
   pthread_mutex_lock(&tqueue->queue_mutex);
   for(i = 0; i < tqueue->wait_count; i++)
     {
        sem_post(&tqueue->data_semaphore);
     }
   pthread_mutex_unlock(&tqueue->queue_mutex);

   /* Wait for threads to leave tqueue_get() */
   while (tqueue->wait_count)
     {
        pthread_mutex_lock(&tqueue->queue_mutex);
        pthread_mutex_unlock(&tqueue->queue_mutex);
     }

   /* Destroy variables and strucutures */
   sem_destroy(&tqueue->data_semaphore);
   pthread_mutex_destroy(&tqueue->queue_mutex);
   queue_destroy(tqueue->queue);
   free(tqueue);
}

/* Push data to tqueue. NULL allowed. */
void
tqueue_push(TQueue *tqueue, const void *data)
{
   if (!tqueue) return;
   pthread_mutex_lock(&tqueue->queue_mutex);
   queue_push(tqueue->queue, data);
   pthread_mutex_unlock(&tqueue->queue_mutex);

   sem_post(&tqueue->data_semaphore);
}

/* Get data from tqueue. */
const void *
tqueue_get(TQueue *tqueue)
{
   const void *ret = NULL;
   /*clean up threads stuk on sem_wait */

   pthread_mutex_lock(&tqueue->queue_mutex);
   tqueue->wait_count++;
   pthread_mutex_unlock(&tqueue->queue_mutex);

   sem_wait(&tqueue->data_semaphore);
   /* If close flag was risen, safel leave tqueue_get(). */
   if (tqueue->close)
     {
        pthread_mutex_lock(&tqueue->queue_mutex);
        tqueue->wait_count--;
        pthread_mutex_unlock(&tqueue->queue_mutex);
        return NULL;
     }

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

