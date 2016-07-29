#ifndef _MATRIX_THREAD_H_
#define _MATRIX_THREAD_H_

#include "matrix.h"

void matrix_mult_thread();

int
matrix_thread_init(int threads_num);

int
matrix_thread_shutdown(void);
#endif
