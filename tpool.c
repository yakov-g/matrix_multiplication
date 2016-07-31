#include <stdlib.h>
#include <pthread.h>

#include "tpool.h"
#include "queue.h"
#include "tqueue.h"
#include "helper.h"

struct _T_Task
{
   void (*task_func) (const void *data);
   const void *data;
};

typedef struct
{
   T_Pool *tpool;
   pthread_t thread;
   size_t count;
} Thread_Data;

struct _T_Pool
{
   int _pool_close;
   Thread_Data *_thread_arr;
   size_t _thread_num;

   int locked;
   pthread_mutex_t _thread_start_mutex;
   pthread_cond_t _thread_start_cond;

   TQueue *tasks;
};

struct _T_Event
{
   size_t counter;
   pthread_mutex_t _event_mutex;
   pthread_cond_t _event_cond;
};

T_Event *
t_event_create(size_t counter)
{
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

void
t_event_destroy(T_Event *te)
{
   if (!te) return;
   pthread_mutex_destroy(&te->_event_mutex);
   pthread_cond_destroy(&te->_event_cond);
   free(te);
}

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

T_Task *
t_task_create(void (*task_func)(const void *data), const void *data)
{
   if (unlikely(!task_func)) return NULL;
   T_Task *tt = (T_Task *) calloc (1, sizeof(T_Task));
   if (unlikely(!tt)) return NULL;
   tt->task_func = task_func;
   tt->data = data;
   return tt;
}

void
t_task_destroy(T_Task *t_task)
{
   if (unlikely(!t_task)) return;
   free(t_task);
}

void
t_pool_task_insert(T_Pool *tpool, const T_Task *task)
{
   if (unlikely(!tpool)) return;
   tqueue_push(tpool->tasks, task);
}

static void *
_thread_func(void *arg)
{
   Thread_Data *td = (Thread_Data *) arg;
   pthread_mutex_lock(&td->tpool->_thread_start_mutex);
   while (td->tpool->locked)
     {
        pthread_cond_wait(&td->tpool->_thread_start_cond, &td->tpool->_thread_start_mutex);
     }
   pthread_mutex_unlock(&td->tpool->_thread_start_mutex);

   while (1)
     {
        if (unlikely(td->tpool->_pool_close))
          {
             pthread_exit(NULL);
          }

        T_Task *t = (T_Task *) tqueue_get(td->tpool->tasks);
        if (likely(t && t->task_func))
          {
             t->task_func(t->data);
             td->count++;
          }
     }
   pthread_exit(NULL);
}

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

size_t
tpool_thread_count_get(const T_Pool *tpool)
{
   if (!tpool) return 0;
   return tpool->_thread_num;
}

void
t_pool_run(T_Pool *tpool)
{
   if (!tpool) return;
   pthread_mutex_lock(&tpool->_thread_start_mutex);
   tpool->locked = 0;
   pthread_cond_broadcast(&tpool->_thread_start_cond);
   pthread_mutex_unlock(&tpool->_thread_start_mutex);
}

int
t_pool_destroy(T_Pool *tpool)
{
   if (!tpool) return 1;

   tpool->_pool_close = 1;
   tqueue_destroy(tpool->tasks);

   size_t i = 0;
   for (i = 0; i < tpool->_thread_num; i++)
     {
        pthread_join(tpool->_thread_arr[i].thread, NULL);
        printf("count: %zu\n", tpool->_thread_arr[i].count);
     }

   pthread_mutex_destroy(&tpool->_thread_start_mutex);
   pthread_cond_destroy(&tpool->_thread_start_cond);

   free(tpool->_thread_arr);
   free(tpool);
   return 0;
}

