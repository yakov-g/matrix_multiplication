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
   return mtr;
}

Matrix *
matrix_from_file_create(const char *filename)
{
   if (!filename) return NULL;
   int n = 0, m = 0, i = 0, j = 0, res;
   FILE *f = fopen(filename, "r");
   long long *matr = NULL;

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
   matr = malloc(n * m * sizeof(long long));
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
             *(matr + m * i +j) = k;
          }
     }

   Matrix *mt = matrix_create(n, m);
   mt->lines = n;
   mt->columns = m;
   mt->data = matr;

end:
   fclose(f);
   return mt;
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
