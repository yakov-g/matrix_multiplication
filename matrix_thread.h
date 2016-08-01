#ifndef _MATRIX_THREAD_H_
#define _MATRIX_THREAD_H_

#include "tpool.h"

/* Multi-threaded matrix multiplication.
 * Runs multiplication in provided T_Pool, returns new matrix.
 * Returns: Matrix* on success, or NULL on fail. */
Matrix *
matrix_mult_thread(T_Pool *tpool, const Matrix *mt1, const Matrix *mt2);

#endif
