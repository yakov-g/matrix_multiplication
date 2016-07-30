#ifndef _T_POOL_H_
#define _T_POOL_H_

typedef struct _T_Task T_Task;
typedef struct _T_Pool T_Pool;

T_Task *
t_task_create(void (*task_func)(const void *data), const void *data);

void
t_task_destroy(T_Task *t_task);

T_Pool*
t_pool_create(int thread_num);

int
t_pool_destroy(T_Pool *tpool);

void
t_pool_task_insert(T_Pool *tpool, const T_Task *task);
#endif


