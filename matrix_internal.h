
#ifndef _MATRIX_INTERNAL_H_
#define _MATRIX_INTERNAL_H_

/*
 * Matrix handler.
 * */
struct _Matrix
{
   unsigned int lines;   /* Number of lines. */
   unsigned int columns; /* Number of columns. */
   long long *data;      /* Pointer to array of data. */
};

#endif
