#include <stdlib.h>
#include <pthread.h>

#include "tpool.h"
#include "ttask.h"
#include "queue.h"
#include "tqueue.h"
#include "helper.h"

/* T_Pool is a generic pthread pool,
 * which manages task queue.
 * Pool creates N of threads and handles task execution
 * in a thread function. */

/* Thread_Data handler which is passed into thread. */
typedef struct
{
   T_Pool *tpool; /* Thread pool. */
   pthread_t thread; /* Thread id. */
   size_t count; /* Not used. Any data can be added to the strucutre. */
} Thread_Data;

/* T_Pool handler. */
struct _T_Pool
{
   /* Array to keep info about threads. */
   Thread_Data *_thread_arr;
   size_t _thread_num; /* Number of threads. */

   /* Threads syncronization. */
   pthread_mutex_t _thread_start_mutex;
   pthread_cond_t _thread_start_cond;

   int locked; /* Flag to lock pool on start */
   int _pool_close /* Pool close flag. */;

   TQueue *tasks; /* Task queue. */
};

/* main thread function. */
static void *
_thread_func(void *arg)
{
   Thread_Data *td = (Thread_Data *) arg;
   /* Lock threads on start, if T_Pool was created with 'locked' flag */
   pthread_mutex_lock(&td->tpool->_thread_start_mutex);
   while (td->tpool->locked)
     {
        pthread_cond_wait(&td->tpool->_thread_start_cond, &td->tpool->_thread_start_mutex);
     }
   pthread_mutex_unlock(&td->tpool->_thread_start_mutex);

   /* Main routine */
   while (1)
     {
        /* Check if pool close flag was rised.  */
        if (unlikely(td->tpool->_pool_close))
          {
             pthread_exit(NULL);
          }

        /* Fetch a task and run callback.  */
        T_Task *t = (T_Task *) tqueue_get(td->tpool->tasks);
        if (likely(t))
          {
             task_func tf = t_task_func_get(t);
             const void *data = t_task_data_get(t);
             tf(data);
          }
     }
   /* Finish thread. */
   pthread_exit(NULL);
}

/* Create T_Pool
 * Returns: T_Pool*, or NULL on fail.  */
T_Pool*
t_pool_create(int thread_num, int locked)
{
   if (thread_num < 1) return NULL;

   T_Pool *tpool = (T_Pool *) calloc(1, sizeof(T_Pool));
   if (!tpool) return NULL;

   tpool->_thread_num = thread_num;
   tpool->locked = locked;

   tpool->_thread_arr = (Thread_Data *) calloc(tpool->_thread_num, sizeof(Thread_Data));
   if (!tpool->_thread_arr)
     {
        goto bad_end;
     }
   if (pthread_mutex_init(&tpool->_thread_start_mutex, NULL))
     {
        goto bad_end;
     }
   if (pthread_cond_init(&tpool->_thread_start_cond, NULL))
     {
        goto bad_end;
     }

   tpool->tasks = tqueue_create();
   if (!tpool->tasks)
      goto bad_end;

   size_t i = 0;
   for (i = 0; i < tpool->_thread_num; i++)
     {
        tpool->_thread_arr[i].tpool = tpool;
        pthread_create(&(tpool->_thread_arr[i].thread), NULL,
                       _thread_func, (void *) &(tpool->_thread_arr[i]));
     }
   return tpool;

bad_end:
   tqueue_destroy(tpool->tasks);
   free(tpool->_thread_arr);
   free(tpool);
   return NULL;
}

/* Destroy T_Pool.
 * Returns: 0 on success, or 1 on fail. */
int
t_pool_destroy(T_Pool *tpool)
{
   if (!tpool) return 1;

   tpool->_pool_close = 1;
   tqueue_destroy(tpool->tasks);

   /* Join all threads. */
   size_t i = 0;
   for (i = 0; i < tpool->_thread_num; i++)
     {
        pthread_join(tpool->_thread_arr[i].thread, NULL);
     }

   pthread_mutex_destroy(&tpool->_thread_start_mutex);
   pthread_cond_destroy(&tpool->_thread_start_cond);

   free(tpool->_thread_arr);
   free(tpool);
   return 0;
}

/* Get count of threads in T_Pool */
size_t
t_pool_thread_count_get(const T_Pool *tpool)
{
   if (!tpool) return 0;
   return tpool->_thread_num;
}

/* Insert a task into T_Pool. */
void
t_pool_task_insert(T_Pool *tpool, const T_Task *task)
{
   if (unlikely(!tpool)) return;
   tqueue_push(tpool->tasks, task);
}

/* Run T_Pool in case if it was created in 'locked' state. */
void
t_pool_run(T_Pool *tpool)
{
   if (!tpool) return;
   pthread_mutex_lock(&tpool->_thread_start_mutex);
   tpool->locked = 0;
   pthread_cond_broadcast(&tpool->_thread_start_cond);
   pthread_mutex_unlock(&tpool->_thread_start_mutex);
}

