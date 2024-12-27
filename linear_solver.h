#ifndef __LINEAR_SOLVER
#include <lapacke.h>
#include <string.h>

/**
 * Solves linear system Ax=b
 * @param double* A input matrix
 * @param int dim_A number of equations, first dimension of A
 * @param double* b rhs matrix, has leading dimension of 1
 * @param double* result stores the resulting vector in result
 * @return int info info related to execution
 *
 */
int linear_system_solver(double A[], int dim_A, double b[], double *result);
#endif // !__LINEAR_SOLVER
