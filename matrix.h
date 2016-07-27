#ifndef _MATRIX_H_
#define _MATRIX_H_

typedef struct
{
   unsigned int lines;
   unsigned int columns;
   long long *data;
} Matrix;

Matrix *
matrix_create(size_t lines, size_t columns);

Matrix*
matrix_from_file_create(const char *filename);

void
matrix_delete(Matrix *mt);

void
matrix_print(Matrix *mt);

long long
vectors_multiply(const long long *v1, const long long *v2, size_t size);

#endif
