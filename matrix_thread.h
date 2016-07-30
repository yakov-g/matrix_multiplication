#ifndef _MATRIX_THREAD_H_
#define _MATRIX_THREAD_H_

Matrix *
matrix_mult_thread(const Matrix *mt1, const Matrix *mt2);

int
thread_pool_init(int threads_num);

int
thread_pool_shutdown(void);
#endif
