#ifndef __TRIDIAG_MATRIX_H
#define __TRIDIAG_MATRIX_H

#include <stdlib.h>
struct tridiagonal_matrix {
	double *du;
	double *d;
	double *dl;
	size_t size;
};

double get_tmatrix_elem(struct tridiagonal_matrix *m, size_t i, size_t j);

int alloc_tmatrix(struct tridiagonal_matrix *m);
void copy_tmatrix(struct tridiagonal_matrix **dest, struct tridiagonal_matrix *src);
void free_tmatrix(struct tridiagonal_matrix *m);
struct tridiagonal_matrix *init_tmatrix(size_t size);

#endif // !__TRIDIAG_MATRIX_H
