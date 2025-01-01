#ifndef __LINEAR_SOLVER_H
#define __LINEAR_SOLVER_H

#include "tridiagonal_matrix.h"

int linear_system_solver(struct tridiagonal_matrix *m, double b[], double *result);
int tridiag_system_solver(struct tridiagonal_matrix *m, double b[], double *result);
#endif // !__LINEAR_SOLVER
