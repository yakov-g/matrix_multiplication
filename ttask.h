#ifndef _T_TASK_H_
#define _T_TASK_H_

typedef struct _T_Task T_Task;
typedef void (*task_func) (const void *data);

T_Task *
t_task_create(void (*task_func)(const void *data), const void *data);

void
t_task_destroy(T_Task *t_task);

task_func
t_task_func_get(const T_Task *task);

const void *
t_task_data_get(const T_Task *task);

#endif
