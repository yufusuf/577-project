#ifndef __CYCLIC_H
#define __CYCLIC_H

#include "tridiagonal_matrix.h"

int cyclic_reduction_seq_low_mem(struct tridiagonal_matrix *A, double b[], double *result);
int cyclic_reduction_seq_high_mem(struct tridiagonal_matrix *A, double b[], double *result);
#endif // !__CYCLIC_H
