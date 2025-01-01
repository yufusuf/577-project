#include "cyclic.h"
#include "tridiagonal_matrix.h"
#include <cblas.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

int cyclic_reduction_seq(struct tridiagonal_matrix *m, double b[], double *result) {
	int i, j, k, stride, index1, index2, offset;
	double alpha, gamma;
	int size = m->size;
	int levels = (size_t)ceil(log2(size + 1));
	double *F = malloc(sizeof(double) * size);
	memcpy(F, b, sizeof(double) * size);
	double *A = calloc(size * size, sizeof(double));
	for (int i = 0; i < size; i++) {
		A[i * size + i] = m->d[i];
		if (i > 0) {
			A[i * size + (i - 1)] = m->dl[i - 1];
		}
		if (i < size - 1) {
			A[i * size + (i + 1)] = m->du[i];
		}
	}

	for (i = 0; i < levels - 1; i++) {
		stride = 1 << (i + 1);
		for (j = stride - 1; j < size; j += stride) {
			offset = (1 << i);
			index1 = j - offset;
			index2 = j + offset;
			alpha = A[index_of(j, size, index1)] / A[index_of(index1, size, index1)];
			gamma = A[index_of(j, size, index2)] / A[index_of(index2, size, index2)];
			for (k = 0; k < size; k++) {
				A[index_of(j, size, k)] -= (alpha * A[index_of(index1, size, k)] + gamma * A[index_of(index2, size, k)]);
			}
			F[j] -= (alpha * F[index1] + gamma * F[index2]);
		}
	}
	int index = (size - 1) / 2;
	result[index] = F[index] / A[index_of(index, size, index)];
	for (i = levels - 2; i >= 0; i--) {
		stride = 1 << (i + 1);
		for (j = stride - 1; j < size; j += stride) {
			offset = (1 << i);
			index1 = j - offset;
			index2 = j + offset;
			result[index1] = F[index1];
			result[index2] = F[index2];
			for (k = 0; k < size; k++) {
				if (k != index1)
					result[index1] -= A[index_of(index1, size, k)] * result[k];
				if (k != index2)
					result[index2] -= A[index_of(index2, size, k)] * result[k];
			}
			result[index1] = result[index1] / A[index_of(index1, size, index1)];
			result[index2] = result[index2] / A[index_of(index2, size, index2)];
		}
	}
	free(A);
	free(F);

	return 0;
}

int cyclic_reduction_parallel(double A[], int dim_A, double *b, double *result) {

	memset(result, 0, sizeof(double) * dim_A);
	return 0;
}
