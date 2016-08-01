#ifndef _MATRIX_H_
#define _MATRIX_H_

typedef struct _Matrix Matrix;

/* Create matrix of (lines x columns) size.
 * Returns: Matrix* on success, NULL on fail. */
Matrix *
matrix_create(size_t lines, size_t columns);

/* Create randomly filled matrix of (lines x columns) size .
 * Returns: Matrix* on success, NULL on fail. */
Matrix *
matrix_random_create(size_t lines, size_t columns);

/* Create matrix with data filled from file.
 * Returns: Matrix* on success, NULL on fail. */
Matrix*
matrix_from_file_create(const char *filename);

/* Save matrix in file.
 * Returns: 0 on success, -1 on fail. */
int
matrix_to_file_save(const Matrix *mt, const char *filename);

/* Transponses matrix. New matrix will be returned.
 * Returns: Matrix* on success, NULL on fail. */
Matrix *
matrix_transponse(const Matrix *mt);

/* Matrix multiplication function.
 * Through transposed mt2. New matrix will be returned.
 * Returns: Matrix* on success, NULL on fail. */
Matrix *
matrix_mult(const Matrix *mt1, const Matrix *mt2);

/* Matrix multiplication function.
 * Without transposing mt2. New matrix will be returned.
 * Returns: Matrix* on success, NULL on fail. */
Matrix *
matrix_no_transpose_mult(const Matrix *mt1, const Matrix *mt2);

/* Matrix compare function.
 * Returns: 0 - if matrixes are equal, 1 - if not equal. */
unsigned int
matrix_cmp(const Matrix *mt1, const Matrix *mt2);

/* Destroy matrix object. */
void
matrix_destroy(Matrix *mt);

/* Print matrix line by line. */
void
matrix_print(Matrix *mt);

/* Multiply two vectors of size.
 * Returns: result of multiplication. */
long long
vectors_multiply(const long long *v1, const long long *v2, size_t size);

#endif
