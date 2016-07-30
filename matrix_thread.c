#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include "matrix.h"
#include "matrix_thread.h"
#include "queue.h"
#include "tpool.h"

static int                 _exit_thread = 0;
static pthread_cond_t      _cond_start  = PTHREAD_COND_INITIALIZER;
static pthread_cond_t      _cond_ready  = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t     _mutex = PTHREAD_MUTEX_INITIALIZER;
static int                 _threads_init_count = 0;
static int                 _thread_num = 0;
static pthread_t           *_thread_arr = NULL;

static int _finish_counter = 0;
typedef struct _Task Task;
typedef struct _Mult_Data Mult_Data;

typedef struct
{
   Task *p;
   Task *pool;
   size_t task_number;

   Queue *q;
} Task_Pool;

static Task_Pool _task_pool;

struct _Task
{
   const long long *v1;
   const long long *v2;
   size_t v_size;
   long long *result_pointer;
   void (*task_func) (const Task *task);
};

struct _Mult_Data
{
   const long long *v1;
   const long long *v2;
   size_t v_size;
   long long *result_pointer;
};

static void
_vect_mult_task_func(const void *data)
{
   Mult_Data *md = (Mult_Data *) data;
   long long res;
   res = vectors_multiply(md->v1, md->v2, md->v_size);
   *md->result_pointer = res;
}

Mult_Data *
mult_data_create(const long long v1, const long long v2, size_t v_size,\
                 long long *res_pointer)
{
   if (!v1 || !v2 || !v_size || res_pointer) return NULL;
   Mult_Data *md = (Mult_Data *) malloc (sizeof(Mult_Data));
   md->v1 = v1;
   md->v2 = v2;
   md->v_size = v_size;
   md->result_pointer = res_pointer;
   return md;
}

void
mult_data_delete(Mult_Data *md)
{
   if (!md) return NULL;
   free(md);
}

static void
_task_vect_mult_func(const Task *task)
{
   long long res;
   res = vectors_multiply(task->v1, task->v2, task->v_size);
   *task->result_pointer = res;
}

static Task*
_pool_task_get()
{
   Task *ret = NULL;
   pthread_mutex_lock(&_mutex);
   if (_task_pool.task_number)
     {
       ret = _task_pool.p;
       _task_pool.p++;
       _task_pool.task_number--;
     }
   pthread_mutex_unlock(&_mutex);
   return ret;
}

static void *
_thread_func(void *arg)
{
   while (1)
     {
        Task *t = _pool_task_get();
        if (t) t->task_func(t);

        if (!t)
          {
             pthread_mutex_lock(&_mutex);
             _finish_counter--;
             //printf("%d: Waiting for job!: %d\n", (int) arg, _finish_counter);
             pthread_cond_broadcast(&_cond_ready);
             pthread_cond_wait(&_cond_start, &_mutex);
             //printf("%d: let's go\n", (int) arg);
             if (_exit_thread)
               {
                  //printf("%d: I'm thread! - Good bye!\n",(int) arg);
                  pthread_mutex_unlock(&_mutex);
                  pthread_exit(NULL);
               }
             pthread_mutex_unlock(&_mutex);
          }
        //printf("I'm thread! - taking data\n");
        //printf("I'm thread! - working\n");
     }
   pthread_exit(NULL);
}

int
thread_pool_init(int thread_num)
{
   if (thread_num < 1) return 0;
   if (_threads_init_count++ > 0)
      return 1;

   _thread_num = thread_num;
   _thread_arr = (pthread_t *) malloc(_thread_num * sizeof(pthread_t));

   int i = 0;
   for (i = 0; i < _thread_num; i++)
     {
        pthread_create(_thread_arr + i, NULL, _thread_func, NULL);
     }
   return 1;
}

int
thread_pool_shutdown(void)
{
   if (--_threads_init_count > 0)
      return 1;

   pthread_mutex_lock(&_mutex);
   pthread_cond_broadcast(&_cond_start);
   _exit_thread = 1;
   pthread_mutex_unlock(&_mutex);

   int i = 0;
   for (i = 0; i < _thread_num; i++)
     {
        pthread_join(_thread_arr[i], NULL);
     }

   free(_thread_arr);
   return 1;
}

void
threads_run()
{
   _finish_counter = _thread_num;
   pthread_mutex_lock(&_mutex);
   pthread_cond_broadcast(&_cond_start);
   pthread_mutex_unlock(&_mutex);
}

void
threads_wait()
{
   pthread_mutex_lock(&_mutex);
   while (_finish_counter > 0)
     {
        pthread_cond_wait(&_cond_ready, &_mutex);
     }
   pthread_mutex_unlock(&_mutex);
}

Matrix *
matrix_mult_thread(const Matrix *mt1, const Matrix *mt2)
{
   if (!mt1 || !mt2) return NULL;
   if (mt1->columns != mt2->lines)
     {
        printf("Matrixes can not be multiplicated");
        return NULL;
     }

#if 0
   Queue *q = queue_create();
   queue_push(q, (void *) 4);
   queue_push(q, 3);
   queue_push(q, 2);
   printf("q: %p\n", queue_peek(q));
   queue_pop(q);
   printf("q: %p\n", queue_peek(q));
   queue_pop(q);
   printf("q: %p\n", queue_peek(q));
   queue_pop(q);
   printf("q: %p\n", queue_peek(q));
   queue_pop(q);
   queue_delete(q);
#endif

   Matrix *res = matrix_create(mt1->lines, mt2->columns);
   Matrix *mt2_trans = matrix_transponse(mt2);

   size_t i, j;
   const long long *v1 = NULL, *v2 = NULL;
   size_t v_size = mt1->columns;

   _task_pool.pool = (Task *) malloc(mt1->lines * mt2->columns * sizeof(Task));
   _task_pool.p = _task_pool.pool;
   _task_pool.q = queue_create();

   size_t task_counter = 0;
   for (i = 0; i < mt1->lines; i++)
     {
        for (j = 0; j < mt2_trans->lines; j++)
          {
             v1 = mt1->data + i * v_size;
             v2 = mt2_trans->data + j * v_size;

             Task *t = _task_pool.pool + task_counter;
             t->v1 = v1;
             t->v2 = v2;
             t->v_size = v_size;
             t->result_pointer = res->data + i * res->columns + j;
             t->task_func = _task_vect_mult_func;
             task_counter++;
          }
     }
   _task_pool.task_number = mt1->lines * mt2->columns;

   threads_run();
   threads_wait();

   matrix_delete(mt2_trans);
   return res;
}
