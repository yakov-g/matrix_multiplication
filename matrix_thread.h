#ifndef _MATRIX_THREAD_H_
#define _MATRIX_THREAD_H_

#include "tpool.h"

Matrix *
matrix_mult_thread(T_Pool *tpool, const Matrix *mt1, const Matrix *mt2);

#endif
