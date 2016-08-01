#include <stdlib.h>
#include "ttask.h"
#include "helper.h"

struct _T_Task
{
   task_func tf;
   const void *data;
};

T_Task *
t_task_create(task_func tf, const void *data)
{
   if (unlikely(!tf)) return NULL;
   T_Task *tt = (T_Task *) calloc (1, sizeof(T_Task));
   if (unlikely(!tt)) return NULL;
   tt->tf = tf;
   tt->data = data;
   return tt;
}

void
t_task_destroy(T_Task *t_task)
{
   if (unlikely(!t_task)) return;
   free(t_task);
}

task_func
t_task_func_get(const T_Task *t_task)
{
   if (unlikely(!t_task)) return NULL;
   return t_task->tf;
}

const void *
t_task_data_get(const T_Task *t_task)
{
   if (unlikely(!t_task)) return NULL;
   return t_task->data;
}
