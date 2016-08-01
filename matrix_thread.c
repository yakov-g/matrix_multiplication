/*
 * Threaded matrix multiplication.
 *
 * Implemented with tasks creation for threads.
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix_internal.h"
#include "matrix.h"
#include "matrix_thread.h"
#include "tpool.h"
#include "tevent.h"
#include "helper.h"

typedef struct _Mult_Data Mult_Data;

/* Struct to keep data for task */
struct _Mult_Data
{
   const long long *v1; /* Ptr to 1st mem region. */
   const long long *v2; /* Ptr to 2nd mem region. */
   size_t v_size1; /* Sizes of memory regions. */
   size_t v_size2;
   size_t v_size; /* Size of vector to muliplicatei. */
   long long *result_pointer; /* Pointer to mem region where to put result. */
   T_Event *te; /* Pointer to T_Event counter. */
   T_Task *task; /* Pointer to current task */
};

/* Helper to create and initialize task data structure */
static Mult_Data *
_mult_data_create(const long long *v1, size_t v_size1,
                   const long long *v2, size_t v_size2,
                   size_t v_size,
                   long long *res_pointer, T_Event *te)
{
   if (unlikely(!v1 || !v_size1 ||
                !v2 || !v_size2 || !v_size || !res_pointer)) return NULL;

   Mult_Data *md = (Mult_Data *) malloc (sizeof(Mult_Data));
   md->v1 = v1;
   md->v_size1 = v_size1;

   md->v2 = v2;
   md->v_size2 = v_size2;

   md->v_size = v_size;
   md->result_pointer = res_pointer;
   md->te = te;
   return md;
}

/* Helper to free task data. */
static void
_mult_data_destroy(Mult_Data *md)
{
   if (unlikely(!md)) return;
   free(md);
}

/* Task function. */
static void
_vect_mult_task_func(const void *data)
{
   Mult_Data *md = (Mult_Data *) data;
   long long res;
   /* Calculate length of 2nd memory area in number of vectors*/
   size_t k = md->v_size2 / md->v_size;
   size_t i = 0;

   /* Initialize iterators for 2nd vector and result. */
   long long *v2_p = (long long *) md->v2;
   long long *res_p = (long long *) md->result_pointer;
   for (i = 0; i < k; i++)
     {
        /* Multiply vectors. */
        res = vectors_multiply(md->v1, v2_p, md->v_size);
        /* assing result. */
        *res_p = res;
        /* Increment pointers to result and 2nd vector*/
        v2_p += md->v_size;
        res_p++;
     }

   /* Decrement task counter */
   t_event_dec(md->te);
   /* Destroy task. */
   t_task_destroy(md->task);
   /* Free task data. */
   _mult_data_destroy(md);
}

/* Multi-threaded matrix multiplication.
 * Runs multiplication in provided T_Pool, by creating set of tasks.
 * Returns new matrix.
 * Returns: Matrix* on success, or NULL on fail. */
Matrix *
matrix_mult_thread(T_Pool *tpool, const Matrix *mt1, const Matrix *mt2)
{
   if (!tpool)
     {
        return NULL;
     }
   if (!mt1 || !mt2) return NULL;
   if (mt1->columns != mt2->lines)
     {
        printf("Matrixes can not be multiplicated\n");
        return NULL;
     }

   /* Transform 2nd matrix for faster operations. */
   Matrix *mt2_trans = matrix_transponse(mt2);
   if (!mt2_trans) return NULL;

   Matrix *res = matrix_create(mt1->lines, mt2->columns);
   if (!res) return NULL;

   size_t i;
   const long long *v1 = NULL, *v2 = NULL;
   size_t v_size = mt1->columns;

   /* Multiply matrixes:
    * Create set of tasks to perform multiplication in
    * following order.
    * 2nd matrix is transposed so multiply
    * line of 1st matrix(M1) by line of 2nd transposed matrix(M2) and so on.
    *
    * In order to decrease cache-misses create tasks in order:
    * 1st line of M1 by 1st half of M2
    * 1st line of M1 by 2nd half of M2
    * 2st line of M1 by 1st half of M2
    * ...etc
    * */

   /* Number of columns (lines in transposed)
    * in each half of 2nd matrix */
   size_t m2_h1_ln = mt2_trans->lines / 2;
   size_t m2_h2_ln = mt2_trans->lines / 2 + mt2_trans->lines % 2;

   /* Size of 1st half of 2nd matrix */
   size_t m2_h1_size = m2_h1_ln * mt2_trans->columns;
   /* Size of 2nd half of 2nd matrix */
   size_t m2_h2_size = m2_h2_ln * mt2_trans->columns;

   /* Create T_Event to count completed tasks.
    * handle special case when M2 is 1 column. */
   T_Event *te = t_event_create(mt1->lines * (mt2_trans->lines == 1 ? 1 : 2));

   /* Create tasks;
    * Lines of M1 x 1st half of M2 */
   if (likely(mt2_trans->lines > 1))
     {
        for (i = 0; i < mt1->lines; i++)
          {
             v1 = mt1->data + i * v_size;
             v2 = mt2_trans->data;

             /* Create task data */
             Mult_Data *md = _mult_data_create(v1, v_size, v2, m2_h1_size,
                                                 v_size, res->data + i * res->columns, te);
             /* Push task func and task data */
             T_Task *tt = t_task_create(_vect_mult_task_func, md);
             /* Add task pointer to task data in order to free task */
             md->task = tt;
             t_pool_task_insert(tpool, tt);
          }
     }

    /* Create tasks:
     * Lines of M1 x 2nd half of M2 */
   for (i = 0; i < mt1->lines; i++)
     {
        v1 = mt1->data + i * v_size;
        v2 = mt2_trans->data + m2_h1_size;

        /* Create task data */
        Mult_Data *md = _mult_data_create(v1, v_size, v2, m2_h2_size,
                                            v_size, res->data + i * res->columns + m2_h1_ln, te);
        /* Push task func and task data */
        T_Task *tt = t_task_create(_vect_mult_task_func, md);
        /* Add task pointer to task data in order to free task */
        md->task = tt;
        t_pool_task_insert(tpool, tt);
     }

   /* Start T_Pool */
   t_pool_run(tpool);
   /* Block until all tasks are finished */
   t_event_wait(te);

   t_event_destroy(te);
   matrix_destroy(mt2_trans);
   return res;
}
