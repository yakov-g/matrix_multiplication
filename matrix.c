#include <stdio.h>

#include "matrix.h"
#include "stdlib.h"

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
   Matrix *mtr = (Matrix *) malloc(sizeof(Matrix));
   mtr->lines = lines;
   mtr->columns = columns;
   mtr->data = malloc(lines * columns * sizeof(long long));
   return mtr;
}

Matrix *
matrix_from_file_create(const char *filename)
{
   if (!filename) return NULL;
   int n = 0, m = 0, i = 0, j = 0, res;
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

   printf("%d %d %d\n", res, n, m);
   Matrix *mt = matrix_create(n, m);
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
             int k;
             res = fscanf(f, "%d", &k);
             if (res != 1) goto end;
             mt->data[m * i + j] = k;
          }
     }

end:
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
  for (i = 0; i < size; i++)
    {
       ret = ret + v1[i] * v2[i];
    }
  return ret;
}
