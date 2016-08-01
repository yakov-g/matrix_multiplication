#ifndef _T_TASK_H_
#define _T_TASK_H_

/* T_Task is a wrapper for data and callback,
 * which is convivient to push into T_Queue and
 * execute in thread function. */

typedef struct _T_Task T_Task;

/* Task callback*/
typedef void (*task_func) (const void *data);

/* Function to create a task.
 * Returns: T_Task*, or NULL on fail. */
T_Task *
t_task_create(void (*task_func)(const void *data), const void *data);

/* Function to destroy a task */
void
t_task_destroy(T_Task *t_task);

/* Function to get pointer to task function.
 * Returns: task_func.*/
task_func
t_task_func_get(const T_Task *task);

/* Function to get task data.
 * Returns: const void* .*/
const void *
t_task_data_get(const T_Task *task);

#endif
