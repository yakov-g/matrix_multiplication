#include <stdlib.h>
#include <pthread.h>

#include "tevent.h"
#include "helper.h"

/* T_Event is synchronization counter,
 * based on pthread.
 * T_Event object is released when counter reaches 0. */

struct _T_Event
{
   size_t counter; /* Counter. */
   pthread_mutex_t _event_mutex; /* Mutex lock access to counter.*/
   pthread_cond_t _event_cond; /* condition variable to release block. */
};

/* Create T_Event object with initialization.
 * Retuns T_Event*, or NULL on fail*/
T_Event *
t_event_create(size_t counter)
{
   if (counter < 1) return NULL;
   T_Event *te = (T_Event *) malloc(sizeof(T_Event));
   if (!te) return NULL;

   if (pthread_mutex_init(&te->_event_mutex, NULL))
     {
        goto bad_end;
     }
   if (pthread_cond_init(&te->_event_cond, NULL))
     {
        goto bad_end;
     }
   te->counter = counter;

   return te;

bad_end:
   free(te);
   return NULL;
}

/* Destroy T_Event object. */
void
t_event_destroy(T_Event *te)
{
   if (!te) return;
   pthread_mutex_destroy(&te->_event_mutex);
   pthread_cond_destroy(&te->_event_cond);
   free(te);
}

/* Block process execution until counter
 * reaches 0. */
void
t_event_wait(T_Event *te)
{
   if (!te) return;
   pthread_mutex_lock(&te->_event_mutex);
   while (te->counter)
     {
        pthread_cond_wait(&te->_event_cond, &te->_event_mutex);
     }
   pthread_mutex_unlock(&te->_event_mutex);
}

/* Decrement counter. */
void
t_event_dec(T_Event *te)
{
   if (unlikely(!te)) return;
   pthread_mutex_lock(&te->_event_mutex);
   if (unlikely(--te->counter == 0))
     {
        pthread_cond_broadcast(&te->_event_cond);
     }
   pthread_mutex_unlock(&te->_event_mutex);
}
