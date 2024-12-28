#ifndef __CYCLIC_H

#include <cblas.h>
#include <string.h>

int cyclic_reduction(double A[], int dim_A, double b[], double *result);
#endif // !__CYCLIC_H
