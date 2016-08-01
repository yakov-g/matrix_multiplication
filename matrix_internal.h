
#ifndef _MATRIX_INTERNAL_H_
#define _MATRIX_INTERNAL_H_

struct _Matrix
{
   unsigned int lines;
   unsigned int columns;
   long long *data;
};

#endif
