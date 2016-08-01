#include <stdlib.h>
#include "ttask.h"
#include "helper.h"

/* T_Task is a wrapper for data and callback,
 * which is convivient to push into T_Queue and
 * execute in thread function. */
struct _T_Task
{
   task_func tf;
   const void *data;
};

/* Function to create a task.
 * Returns: T_Task*, or NULL on fail. */
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

/* Function to destroy a task */
void
t_task_destroy(T_Task *t_task)
{
   if (unlikely(!t_task)) return;
   free(t_task);
}

/* Function to get pointer to task function.
 * Returns: task_func.*/
task_func
t_task_func_get(const T_Task *t_task)
{
   if (unlikely(!t_task)) return NULL;
   return t_task->tf;
}

/* Function to get task data.
 * Returns: const void* .*/
const void *
t_task_data_get(const T_Task *t_task)
{
   if (unlikely(!t_task)) return NULL;
   return t_task->data;
}
