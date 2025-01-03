#include "cyclic.h"
#include "tridiagonal_matrix.h"
#include <cblas.h>
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cyclic_reduction_seq(struct tridiagonal_matrix *m, double b[], double *result) {
	int i, j, stride, index1, index2, offset;
	double alpha, gamma;
	int size = m->size;
	int levels = (size_t)ceil(log2(size + 1));
	double *F = malloc(sizeof(double) * size);
	memcpy(F, b, sizeof(double) * size);
	memset(result, 0, sizeof(double) * size);
	struct tridiagonal_matrix *A;
	copy_tmatrix(&A, m);
	for (i = 0; i < levels - 1; i++) {
		stride = 1 << (i + 1);
		for (j = stride - 1; j < size; j += stride) {
			offset = (1 << i);
			index1 = j - offset;
			index2 = j + offset;
			alpha = A->dl[j] / A->d[index1];
			gamma = A->du[j] / A->d[index2];

			A->dl[j] = -A->dl[j] * alpha;
			A->d[j] = A->d[j] - A->du[index1] * alpha - A->dl[index2] * gamma;
			A->du[j] = -A->du[index2] * gamma;

			// alpha = A[index_of(j, size, index1)] / A[index_of(index1, size, index1)];
			// gamma = A[index_of(j, size, index2)] / A[index_of(index2, size, index2)];
			// for (k = 0; k < size; k++) {
			// 	A[index_of(j, size, k)] -= (alpha * A[index_of(index1, size, k)] + gamma * A[index_of(index2, size, k)]);
			// }
			F[j] -= (alpha * F[index1] + gamma * F[index2]);
		}
	}
	int index = (size - 1) / 2;
	result[index] = F[index] / A->d[index];
	for (i = levels; i >= 0; i--) {
		stride = 1 << (i + 1);
		for (j = stride - 1; j < size; j += stride) {
			offset = (1 << i);
			index1 = j - offset;
			index2 = j + offset;
			if (j != index1) {
				if (index1 - offset < 0)
					result[index1] = (F[index1] - A->du[index1] * result[index1 + offset]) / A->d[index1];
				else
					result[index1] = (F[index1] - A->dl[index1] * result[index1 - offset] - A->du[index1] * result[index1 + offset]) / A->d[index1];
			}
			if (j != index2) {
				if (index2 + offset >= size)
					result[index2] = (F[index2] - A->dl[index2] * result[j]) / A->d[index2];
				else
					result[index2] = (F[index2] - A->dl[index2] * result[j] - A->du[index2] * result[index2 + offset]) / A->d[index2];
			}
			// result[index1] = F[index1];
			// result[index2] = F[index2];
			//
			// for (k = 0; k < size; k++) {
			// 	if (k != index1)
			// 		result[index1] -= A[index_of(index1, size, k)] * result[k];
			// 	if (k != index2)
			// 		result[index2] -= A[index_of(index2, size, k)] * result[k];
			// }
			// result[index1] = result[index1] / A[index_of(index1, size, index1)];
			// result[index2] = result[index2] / A[index_of(index2, size, index2)];
		}
	}
	free_tmatrix(A);
	free(F);

	return 0;
}

int cyclic_reduction_parallel(double A[], int dim_A, double *b, double *result) {

	memset(result, 0, sizeof(double) * dim_A);
	return 0;
}
