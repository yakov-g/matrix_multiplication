#ifndef _MATRIX_THREAD_H_
#define _MATRIX_THREAD_H_

Matrix *
matrix_mult_thread(const Matrix *mt1, const Matrix *mt2);

int
matrix_thread_init(int threads_num);

int
matrix_thread_shutdown(void);
#endif
