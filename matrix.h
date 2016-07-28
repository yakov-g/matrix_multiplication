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

Matrix *
matrix_transponse(const Matrix *mt);

Matrix *
matrix_mult(const Matrix *mt1, const Matrix *mt2);

Matrix *
matrix_no_transpose_mult(const Matrix *mt1, const Matrix *mt2);

unsigned int
matrix_cmp(const Matrix *mt1, const Matrix *mt2);

void
matrix_delete(Matrix *mt);

void
matrix_print(Matrix *mt);

long long
vectors_multiply(const long long *v1, const long long *v2, int size);

#endif
