#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix.h"
#include "matrix_thread.h"
#include "tpool.h"
#include "helper.h"

typedef struct _Mult_Data Mult_Data;

struct _Mult_Data
{
   const long long *v1;
   const long long *v2;
   size_t v_size1;
   size_t v_size2;
   size_t v_size;
   long long *result_pointer;
   T_Event *te;
   T_Task *task;
};

static Mult_Data *
_mult_data_create2(const long long *v1, size_t v_size1,
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

static void
_mult_data_destroy2(Mult_Data *md)
{
   if (unlikely(!md)) return;
   free(md);
}

static void
_vect_mult_task_func(const void *data)
{
   Mult_Data *md = (Mult_Data *) data;
   long long res;
   size_t k = md->v_size2 / md->v_size1;
   size_t i = 0;

   long long *v2_p = (long long *) md->v2;
   long long *res_p = (long long *) md->result_pointer;
   for (i = 0; i < k; i++)
     {
        res = vectors_multiply(md->v1, v2_p, md->v_size1);
        *res_p = res;
        v2_p += md->v_size1;
        res_p++;
     }

   t_event_dec(md->te);
   t_task_destroy(md->task);
   _mult_data_destroy2(md);
}

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
        printf("Matrixes can not be multiplicated");
        return NULL;
     }

   Matrix *res = matrix_create(mt1->lines, mt2->columns);
   if (!res) return NULL;

   clock_t start, end;
   start = clock();
   Matrix *mt2_trans = matrix_transponse(mt2);
   end = clock();
   printf("Trans Time: %ld - %ld = %ld\n", end, start, end - start);

   size_t i;
   const long long *v1 = NULL, *v2 = NULL;
   size_t v_size = mt1->columns;

   /* number of columns in each half of second matrix,
    * the same as lines in m2 trans */
   size_t m2_h1_ln = mt2_trans->lines / 2;
   size_t m2_h2_ln = mt2_trans->lines / 2 + mt2_trans->lines % 2;

   /* 1st half 2nd matrix size */
   size_t m2_h1_size = m2_h1_ln * mt2_trans->columns;
   size_t m2_h2_size = m2_h2_ln * mt2_trans->columns;

   T_Event *te = t_event_create(mt1->lines * (mt2_trans->lines == 1 ? 1 : 2));
   if (likely(mt2_trans->lines > 1))
     {
        for (i = 0; i < mt1->lines; i++)
          {
             v1 = mt1->data + i * v_size;
             v2 = mt2_trans->data;

             Mult_Data *md = _mult_data_create2(v1, v_size, v2, m2_h1_size,
                                                 v_size, res->data + i * res->columns, te);
             T_Task *tt = t_task_create(_vect_mult_task_func, md);
             md->task = tt;
             t_pool_task_insert(tpool, tt);
          }
     }

   for (i = 0; i < mt1->lines; i++)
     {
        v1 = mt1->data + i * v_size;
        v2 = mt2_trans->data + m2_h1_size;

        Mult_Data *md = _mult_data_create2(v1, v_size, v2, m2_h2_size,
                                            v_size, res->data + i * res->columns + m2_h1_ln, te);
        T_Task *tt = t_task_create(_vect_mult_task_func, md);
        md->task = tt;
        t_pool_task_insert(tpool, tt);
     }

   start = clock();
   t_pool_run(tpool);
   t_event_wait(te);
   end = clock();
   printf("Mult Time: %ld - %ld = %ld\n", end, start, end - start);

   t_event_destroy(te);
   matrix_delete(mt2_trans);
   return res;
}
