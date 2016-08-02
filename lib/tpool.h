#ifndef _T_POOL_H_
#define _T_POOL_H_

#include "ttask.h"

/* T_Pool is a generic pthread pool,
 * which manages task queue.
 * Pool creates N of threads and handles task execution
 * in a thread function. */

typedef struct _T_Pool T_Pool;

/* Create T_Pool
 * Returns: T_Pool*, or NULL on fail.  */
T_Pool*
t_pool_create(int thread_num, int locked);

/* Destroy T_Pool.
 * Returns: 0 on success, or 1 on fail. */
int
t_pool_destroy(T_Pool *tpool);

/* Get count of threads in T_Pool */
size_t
t_pool_thread_count_get(const T_Pool *tpool);

/* Insert a task into T_Pool. */
void
t_pool_task_insert(T_Pool *tpool, const T_Task *task);

/* Run T_Pool in case if it was created in 'locked' state. */
void
t_pool_run(T_Pool *tpool);

#endif


