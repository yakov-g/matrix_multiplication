#include <stdlib.h>
#include "tpool.h"

struct _T_Task
{
   void (*task_func) (const void *data);
   const void *data;
};

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
