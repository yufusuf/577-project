#ifndef __CYCLIC_H

#include <cblas.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

#define index_of(...) index_(__VA_ARGS__)
#define index_(i, size, j) ((i) * (size) + (j))

int cyclic_reduction_seq(double A[], int dim_A, double b[], double *result);
int cyclic_reduction_parallel(double A[], int dim_A, double b[],
			      double *result);
#endif // !__CYCLIC_H
