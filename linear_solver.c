#include "linear_solver.h"
#include <lapacke.h>
#include <stdlib.h>
#include <string.h>

int linear_system_solver(struct tridiagonal_matrix *m, double b[], double *result) {
	int info;
	size_t size = m->size;
	int pivots[size];
	memcpy(result, b, size * sizeof(double));
	// form nxn matrix;
	double *A = calloc(size * size, sizeof(double));
	for (size_t i = 0; i < size; i++) {
		A[i * size + i] = m->d[i];
		if (i > 0) {
			A[i * size + (i - 1)] = m->dl[i - 1];
		}
		if (i < size - 1) {
			A[i * size + (i + 1)] = m->du[i];
		}
	}
	info = LAPACKE_dgesv(LAPACK_ROW_MAJOR, size, 1, A, size, pivots, result, 1);
	free(A);
	return info;
}
int tridiag_system_solver(struct tridiagonal_matrix *m, double b[], double *result) {
	int info;
	struct tridiagonal_matrix *temp = NULL;

	memcpy(result, b, m->size * sizeof(double));
	copy_tmatrix(&temp, m);

	info = LAPACKE_dgtsv(LAPACK_ROW_MAJOR, temp->size, 1, temp->dl, temp->d, temp->du, result, 1);

	free_tmatrix(temp);
	return info;
}
