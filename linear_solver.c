#include "linear_solver.h"
#include "aux.h"
#include "tridiagonal_matrix.h"
#include <lapacke.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int linear_system_solver(struct tridiagonal_matrix *m, double b[], double *result) {
	int info;
	size_t size = m->size;
	int pivots[size];
	memcpy(result, b, size * sizeof(double));
	double *A = calloc(size * size, sizeof(double));
	convert_to_nxn(m, A);
	info = LAPACKE_dgesv(LAPACK_ROW_MAJOR, size, 1, A, size, pivots, result, 1);
	free(A);
	return info;
}
int tridiag_system_solver(struct tridiagonal_matrix *m, double b[], double *result) {
	int info;
	struct tridiagonal_matrix *temp = NULL;

	memcpy(result, b, m->size * sizeof(double));
	copy_tmatrix(&temp, m);

	info = LAPACKE_dgtsv(LAPACK_ROW_MAJOR, temp->size, 1, temp->dl + 1, temp->d, temp->du, result, 1);
	// print_vect(result, m->size);

	free_tmatrix(temp);
	return info;
}
