#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "matrix.h"

void
matrix_print(Matrix *mt)
{
   if (!mt) return;
   size_t i = 0, j = 0;
   for (i = 0; i < mt->lines; i++)
     {
        for (j = 0; j < mt->columns; j++)
          {
             printf("%lld ", mt->data[mt->columns * i + j]);
          }
        printf("\n");
     }
}

Matrix *
matrix_create(size_t lines, size_t columns)
{
   if (lines <= 0 || columns <= 0) return NULL;
   Matrix *mtr = (Matrix *) malloc(sizeof(Matrix));
   mtr->lines = lines;
   mtr->columns = columns;
   mtr->data = malloc(lines * columns * sizeof(long long));
   return mtr;
}

Matrix *
matrix_random_create(size_t lines, size_t columns)
{
   Matrix *mtr = matrix_create(lines, columns);
   if (!mtr) return NULL;

   srand48(time(NULL));

   size_t i;
   for (i = 0; i < lines * columns; i++)
     {
        mtr->data[i] = (long long) lrand48();
     }
   return mtr;
}

Matrix *
matrix_from_file_create(const char *filename)
{
   if (!filename) return NULL;
   int n = 0, m = 0, i = 0, j = 0, res;
   int scan_count = 0;
   Matrix *mt = NULL;

   FILE *f = fopen(filename, "r");

   if (!f)
     {
        printf("Can not open file: \"%s\".\n", filename);
        return NULL;
     }

   res = fscanf(f, "%d %d ", &n, &m);
   if (res != 2)
     {
        goto end;
     }

   mt = matrix_create(n, m);
   if (!mt) goto end;
   for (i = 0; i < n; i++)
     {
#if 0
        char *str = NULL;
        size_t n = 0;
        getline(&str, &n, f);
        printf(":%s", str);
        free(str);
        str = NULL;
#endif
        for (j = 0; j < m; j++)
          {
             res = fscanf(f, "%lld", &mt->data[m * i + j]);
             if (res != 1) goto end;
             scan_count += res;
          }
     }

end:
   if (scan_count != m * n)
     {
        matrix_delete(mt);
        mt = NULL;
        printf("Wrong number of elements\n");
     }
   fclose(f);
   return mt;
}

Matrix *
matrix_transponse(const Matrix *mt)
{
   if (!mt) return NULL;
   Matrix *mt_trans = matrix_create(mt->columns, mt->lines);
   size_t i, j;
   for (i = 0; i < mt->lines; i++)
     {
        for (j = 0; j < mt->columns; j++)
          {
             mt_trans->data[j * mt->lines + i] = mt->data[i * mt->columns + j];
          }
     }
   return mt_trans;
}

Matrix *
matrix_mult(const Matrix *mt1, const Matrix *mt2)
{
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

             res->data[i * res->columns + j] = \
                vectors_multiply(v1, v2, v_size);
          }
     }
   matrix_delete(mt2_trans);
   return res;
}

Matrix *
matrix_no_transpose_mult(const Matrix *mt1, const Matrix *mt2)
{
   if (!mt1 || !mt2) return NULL;
   if (mt1->columns != mt2->lines)
     {
        printf("Matrixes can not be multiplicated");
        return NULL;
     }

   Matrix *res = matrix_create(mt1->lines, mt2->columns);

   size_t i, j, k;
   size_t v_size = mt1->columns;
   for (i = 0; i < mt1->lines; i++)
     {
        for (j = 0; j < mt2->columns; j++)
          {
             long long m = 0;
             for (k = 0; k < v_size; k++)
               {
                  m += mt1->data[i * v_size + k] * mt2->data[k * mt2->columns + j];
               }

             res->data[i * res->columns + j] = m;
          }
     }
   return res;
}

unsigned int
matrix_cmp(const Matrix *mt1, const Matrix *mt2)
{
   if (!mt1 || !mt2) return 1;
   if (mt1 == mt2) return 0;
   if (mt1->lines != mt2->lines ||
       mt1->columns != mt2->columns) return 1;
   return !!memcmp(mt1->data, mt2->data, mt1->lines * mt1->columns * sizeof(long long));
}

void
matrix_delete(Matrix *mt)
{
   if(!mt) return;
   if (mt->data) free(mt->data);
   free(mt);
}

long long
vectors_multiply(const long long *v1, const long long *v2, size_t size)
{
   long long ret = 0;
   size_t i = 0;
   if (!v1 || !v2)
     {
        printf("v1 or v2 is null: %p %p\n", v1, v2);
        return 0;
     }
#if 0
  for (i = 0; i < size; i++)
    {
       ret = ret + (*v1++) * (*v2++);
    }
#else
  /* this optimization does not help with compiler option -O3 */
  if (size > 7)
    {
       for (i = 0; i <= size - 8; i+= 8)
         {
            ret = ret + (*v1++) * (*v2++);
            ret = ret + (*v1++) * (*v2++);
            ret = ret + (*v1++) * (*v2++);
            ret = ret + (*v1++) * (*v2++);
            ret = ret + (*v1++) * (*v2++);
            ret = ret + (*v1++) * (*v2++);
            ret = ret + (*v1++) * (*v2++);
            ret = ret + (*v1++) * (*v2++);
         }
    }
  for (; i < size; i++)
    {
       ret = ret + (*v1++) * (*v2++);
    }
#endif
  return ret;
}
