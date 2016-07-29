#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include "matrix.h"
#include "matrix_thread.h"

static int                 _work_to_do = 0;
static int                 _exit_thread = 0;
static pthread_cond_t      _cond  = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t     _mutex = PTHREAD_MUTEX_INITIALIZER;
static int                 _threads_init_count = 0;
static int                 _thread_num = 0;
static pthread_t           *_thread_arr = NULL;

static void *
_thread_func(void *arg)
{
   while (1)
     {
        pthread_mutex_lock(&_mutex);
        while (!_work_to_do)
          {
             printf("Waiting for job!\n");
             pthread_cond_wait(&_cond, &_mutex);
             if (_exit_thread)
               {
                  printf("I'm thread! - Good bye!\n");
                  pthread_mutex_unlock(&_mutex);
                  pthread_exit(NULL);
               }
          }
        printf("I'm thread! - taking data\n");
        pthread_mutex_unlock(&_mutex);
        printf("I'm thread! - working\n");
     }
   pthread_exit(NULL);
}

int
matrix_thread_init(int thread_num)
{
   if (thread_num < 1) return 0;
   if (_threads_init_count++ > 0)
      return 1;

   _thread_num = thread_num;
   _thread_arr = malloc(_thread_num * sizeof(pthread_t));


   int i = 0;
   for (i = 0; i < _thread_num; i++)
     {
        pthread_create(_thread_arr + i, NULL, _thread_func, NULL);
     }

   return 1;
}

int
matrix_thread_shutdown(void)
{
   if (--_threads_init_count > 0)
      return 1;


   pthread_mutex_lock(&_mutex);
   _exit_thread = 1;

   pthread_cond_broadcast(&_cond);
   pthread_mutex_unlock(&_mutex);
   int i = 0;
   for (i = 0; i < _thread_num; i++)
     {
        pthread_join(_thread_arr[i], NULL);
     }

   return 1;
}


void matrix_mult_thread()
{
}
