#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "matrix.h"
#include "matrix_thread.h"
#include "tpool.h"

typedef struct _Mult_Data Mult_Data;

struct _Mult_Data
{
   const long long *v1;
   const long long *v2;
   size_t v_size;
   long long *result_pointer;
};

static void
_vect_mult_task_func(const void *data)
{
   Mult_Data *md = (Mult_Data *) data;
   long long res;
   res = vectors_multiply(md->v1, md->v2, md->v_size);
   *md->result_pointer = res;
}

Mult_Data *
mult_data_create(const long long *v1, const long long *v2, size_t v_size,\
                 long long *res_pointer)
{
   if (!v1 || !v2 || !v_size || !res_pointer) return NULL;
   Mult_Data *md = (Mult_Data *) malloc (sizeof(Mult_Data));
   md->v1 = v1;
   md->v2 = v2;
   md->v_size = v_size;
   md->result_pointer = res_pointer;
   return md;
}

void
mult_data_delete(Mult_Data *md)
{
   if (!md) return;
   free(md);
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
   Matrix *mt2_trans = matrix_transponse(mt2);

   size_t i, j;
   const long long *v1 = NULL, *v2 = NULL;
   size_t v_size = mt1->columns;

   for (i = 0; i < mt1->lines; i++)
     {
        for (j = 0; j < mt2_trans->lines; j++)
          {
             v1 = mt1->data + i * v_size;
             v2 = mt2_trans->data + j * v_size;

             Mult_Data *md = mult_data_create(v1, v2, v_size, res->data + i * res->columns + j);
             T_Task *tt = t_task_create(_vect_mult_task_func, md);

             t_pool_task_insert(tpool, tt);
          }
     }

   threads_run(tpool);
   threads_wait(tpool);

   matrix_delete(mt2_trans);
   return res;
}
