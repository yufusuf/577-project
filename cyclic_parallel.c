#include "aux.h"
#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv) {
	int i, j, k, size, index;
	int index1, index2;
	int mynode, totalnodes;
	double alpha, gamma;
	const int numrows = 5;
	MPI_Status status;
	printf("hello\n");
	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &totalnodes);

	MPI_Comm_rank(MPI_COMM_WORLD, &mynode);
	printf("Hello from process %d out of %d\n", mynode, totalnodes);

	// Finalize MPI
	MPI_Finalize();

	return 0;
}
