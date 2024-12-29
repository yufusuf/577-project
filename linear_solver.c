#include "linear_solver.h"
#include <string.h>

int linear_system_solver(double A[], int dim_A, double b[], double *result) {
	int info;
	int pivots[dim_A];
	memcpy(result, b, dim_A * sizeof(double));
	info = LAPACKE_dgesv(LAPACK_ROW_MAJOR, dim_A, 1, A, dim_A, pivots,
			     result, 1);
	return info;
}
