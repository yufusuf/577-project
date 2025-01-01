#include "tridiagonal_matrix.h"
#include <stdlib.h>
#include <string.h>

struct tridiagonal_matrix *init_tmatrix(size_t size) {
	struct tridiagonal_matrix *new = malloc(sizeof(struct tridiagonal_matrix));
	new->size = size;
	return new;
}
int alloc_tmatrix(struct tridiagonal_matrix *m) {
	m->du = malloc(sizeof(double) * (m->size - 1));
	m->d = malloc(sizeof(double) * (m->size));
	m->dl = malloc(sizeof(double) * (m->size - 1));
	return 0;
}

double get_tmatrix_elem(struct tridiagonal_matrix *m, size_t i, size_t j) {
	if (i == j) {
		return m->d[i];
	} else if (i == j + 1) {
		return m->dl[j];
	} else if (j == i + 1) {
		return m->du[i];
	} else {
		return 0.0;
	}
}

void copy_tmatrix(struct tridiagonal_matrix **dest, struct tridiagonal_matrix *src) {
	size_t size = src->size;
	*dest = malloc(sizeof(struct tridiagonal_matrix));
	(*dest)->size = size;
	alloc_tmatrix(*dest);
	memcpy((*dest)->d, src->d, sizeof(double) * size);
	memcpy((*dest)->du, src->du, sizeof(double) * (size - 1));
	memcpy((*dest)->dl, src->dl, sizeof(double) * (size - 1));
}

void free_tmatrix(struct tridiagonal_matrix *m) {
	free(m->du);
	free(m->d);
	free(m->dl);
	free(m);
}
