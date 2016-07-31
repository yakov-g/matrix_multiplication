#include <stdlib.h>
#include <pthread.h>
#include "tpool.h"
#include "queue.h"
#include "tqueue.h"

struct _T_Task
{
   void (*task_func) (const void *data);
   const void *data;
};

struct _T_Pool
{
   int _pool_close;
   pthread_t *_thread_arr;
   size_t _thread_num;

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
   if (!te) return;
   pthread_mutex_lock(&te->_event_mutex);
   if (--te->counter == 0)
     {
        pthread_cond_broadcast(&te->_event_cond);
     }
   pthread_mutex_unlock(&te->_event_mutex);
}

T_Task *
t_task_create(void (*task_func)(const void *data), const void *data)
{
   if (!task_func) return NULL;
   T_Task *tt = (T_Task *) calloc (1, sizeof(T_Task));
   if (!tt) return NULL;
   tt->task_func = task_func;
   tt->data = data;
   return tt;
}

void
t_task_destroy(T_Task *t_task)
{
   if (!t_task) return;
   free(t_task);
}

void
t_pool_task_insert(T_Pool *tpool, const T_Task *task)
{
   if (!tpool) return;
   tqueue_push(tpool->tasks, task);
}

static void *
_thread_func(void *arg)
{
   T_Pool *tpool = (T_Pool *) arg;
   while (1)
     {
        if (tpool->_pool_close)
          {
             pthread_exit(NULL);
          }

        T_Task *t = (T_Task *) tqueue_get(tpool->tasks);
        if (t && t->task_func)
          {
             t->task_func(t->data);
          }
     }
   pthread_exit(NULL);
}

T_Pool*
t_pool_create(int thread_num)
{
   if (thread_num < 1) return NULL;

   T_Pool *tpool = (T_Pool *) calloc(1, sizeof(T_Pool));
   if (!tpool) return NULL;

   tpool->_thread_num = thread_num;

   tpool->_thread_arr = (pthread_t *) malloc(tpool->_thread_num * sizeof(pthread_t));
   if (!tpool->_thread_arr)
     {
        goto bad_end;
     }

   tpool->tasks = tqueue_create();
   if (!tpool->tasks)
      goto bad_end;

   size_t i = 0;
   for (i = 0; i < tpool->_thread_num; i++)
     {
        pthread_create(&tpool->_thread_arr[i], NULL, _thread_func, (void *) tpool);
     }
   return tpool;

bad_end:
   exit(0);
   tqueue_destroy(tpool->tasks);
   free(tpool->_thread_arr);
   free(tpool);
   return NULL;
}

int
t_pool_destroy(T_Pool *tpool)
{
   if (!tpool) return 1;

   tpool->_pool_close = 1;
   printf("Close pool 0\n");
   tqueue_destroy(tpool->tasks);

   size_t i = 0;
   printf("Close pool 1\n");
   for (i = 0; i < tpool->_thread_num; i++)
     {
        pthread_join(tpool->_thread_arr[i], NULL);
     }
   printf("Close pool 2\n");

   free(tpool->_thread_arr);
   free(tpool);
   return 0;
}

