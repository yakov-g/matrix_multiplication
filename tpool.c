#include <stdlib.h>
#include <pthread.h>
#include "tpool.h"
#include "queue.h"

struct _T_Task
{
   void (*task_func) (const void *data);
   const void *data;
};

struct _T_Pool
{
   int _exit_thread;
   int _finish_counter;
   pthread_mutex_t _mutex;
   pthread_cond_t _cond_start;
   pthread_cond_t _cond_ready;
   pthread_t *_thread_arr;
   int _thread_num;

   Queue *tasks;
};

static const void*
_pool_task_get(T_Pool *tpool);

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
t_task_delete(T_Task *t_task)
{
   if (!t_task) return;
   free(t_task);
}

void
t_pool_task_insert(T_Pool *tpool, const T_Task *task)
{
   if (!tpool) return;
   if (!task) return;
   pthread_mutex_lock(&tpool->_mutex);
   queue_push(tpool->tasks, task);
   pthread_mutex_unlock(&tpool->_mutex);
}

static void *
_thread_func(void *arg)
{
   T_Pool *tpool = (T_Pool *) arg;
   while (1)
     {
        T_Task *t = (T_Task *) _pool_task_get(tpool);
        if (t && t->task_func) t->task_func(t->data);

        if (!t)
          {
             pthread_mutex_lock(&tpool->_mutex);
             tpool->_finish_counter--;
             //printf("%d: Waiting for job!: %d\n", (int) arg, _finish_counter);
             pthread_cond_broadcast(&tpool->_cond_ready);
             pthread_cond_wait(&tpool->_cond_start, &tpool->_mutex);
             //printf("%d: let's go\n", (int) arg);
             if (tpool->_exit_thread)
               {
                  //printf("%d: I'm thread! - Good bye!\n",(int) arg);
                  pthread_mutex_unlock(&tpool->_mutex);
                  pthread_exit(NULL);
               }
             pthread_mutex_unlock(&tpool->_mutex);
          }
        //printf("I'm thread! - taking data\n");
        //printf("I'm thread! - working\n");
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

   tpool->tasks = queue_create();
   if (!tpool->tasks)
      goto bad_end;

   if (pthread_mutex_init(&tpool->_mutex, NULL))
     {
        goto bad_end;
     }
   if (pthread_cond_init(&tpool->_cond_start, NULL))
     {
        goto bad_end;
     }
   if (pthread_cond_init(&tpool->_cond_ready, NULL))
     {
        goto bad_end;
     }

   int i = 0;
   for (i = 0; i < tpool->_thread_num; i++)
     {
        pthread_create(&tpool->_thread_arr[i], NULL, _thread_func, (void *) tpool);
     }
   return tpool;

bad_end:
   exit(0);
   queue_delete(tpool->tasks);
   free(tpool->_thread_arr);
   free(tpool);
   return NULL;
}

int
t_pool_shutdown(T_Pool *tpool)
{
   if (!tpool) return 1;

   pthread_mutex_lock(&tpool->_mutex);
   pthread_cond_broadcast(&tpool->_cond_start);
   tpool->_exit_thread = 1;
   pthread_mutex_unlock(&tpool->_mutex);

   int i = 0;
   for (i = 0; i < tpool->_thread_num; i++)
     {
        pthread_join(tpool->_thread_arr[i], NULL);
     }

   pthread_mutex_destroy(&tpool->_mutex);
   pthread_cond_destroy(&tpool->_cond_start);
   pthread_cond_destroy(&tpool->_cond_ready);

   queue_delete(tpool->tasks);
   free(tpool->_thread_arr);
   free(tpool);
   return 0;
}

void
threads_run(T_Pool *tpool)
{
   tpool->_finish_counter = tpool->_thread_num;
   pthread_mutex_lock(&tpool->_mutex);
   pthread_cond_broadcast(&tpool->_cond_start);
   pthread_mutex_unlock(&tpool->_mutex);
}

void
threads_wait(T_Pool *tpool)
{
   pthread_mutex_lock(&tpool->_mutex);
   while (tpool->_finish_counter > 0)
     {
        pthread_cond_wait(&tpool->_cond_ready, &tpool->_mutex);
     }
   pthread_mutex_unlock(&tpool->_mutex);
}

static const void*
_pool_task_get(T_Pool *tpool)
{
   const void *ret = NULL;
   pthread_mutex_lock(&tpool->_mutex);
#if 0
   if (_task_pool.task_number)
     {
       ret = *_task_pool.p;
       _task_pool.p++;
       _task_pool.task_number--;
     }
#else
   ret = queue_peek(tpool->tasks);
   if (ret)
     {
        queue_pop(tpool->tasks);
     }
#endif
   pthread_mutex_unlock(&tpool->_mutex);
   return ret;
}
