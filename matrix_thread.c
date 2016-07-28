#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>

#include "matrix.h"

int                 work_to_do = 0;
int                 exit_thread = 0;
pthread_cond_t      cond  = PTHREAD_COND_INITIALIZER;
pthread_mutex_t     mutex = PTHREAD_MUTEX_INITIALIZER;

static void *
_thread_func(void *arg)
{
   while (1)
     {
        pthread_mutex_lock(&mutex);
        while (!work_to_do)
          {
             printf("Waiting for job!\n");
             pthread_cond_wait(&cond, &mutex);
             if (exit_thread)
               {
                  printf("I'm thread! - Good bye!\n");
                  pthread_mutex_unlock(&mutex);
                  pthread_exit(NULL);
               }
          }
        printf("I'm thread! - taking data\n");
        pthread_mutex_unlock(&mutex);
        printf("I'm thread! - working\n");
     }
   pthread_exit(NULL);
}

void matrix_mult_thread()
{
   pthread_t t;
   pthread_create(&t, NULL, _thread_func, NULL);
   sleep(2);

   pthread_mutex_lock(&mutex);
   exit_thread = 1;

   pthread_cond_broadcast(&cond);
   pthread_mutex_unlock(&mutex);
   pthread_join(t, NULL);
}
