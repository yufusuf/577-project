#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

void forward_multi(double *a, double *b, double *c, double *r, int size_per_process, int nprocs, int mynode);
void forward_single(double *a, double *b, double *c, double *x, double *r, int size_per_process, int nprocs, int mynode);
void backsubstitution(double *a, double *b, double *c, double *x, double *r, int size_per_process, int nprocs, int mynode);
void write_results(double *solution, double *rhs, int matrix_size, double *time);
int main(int argc, char **argv) {
	int nprocs;
	int mynode;
	int size_per_process;
	int matrix_size;
	double *a, *b, *c, *x, *r;
	double *a_ver, *b_ver, *c_ver, *r_ver;
	double start_time, elapsed_time;
	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &mynode);
	if (argc < 2) {
		if (mynode == 0) {
			printf("give matrix size as argument as a power of 2\n");
		}
		MPI_Finalize();
		return EXIT_FAILURE;
	}
	srand(time(NULL));

	matrix_size = atoi(argv[1]);

	size_per_process = matrix_size / nprocs;

	a = malloc(sizeof(double) * (size_per_process + 2));	 // upper diag
	a_ver = malloc(sizeof(double) * (size_per_process + 2)); // upper diag

	b = malloc(sizeof(double) * (size_per_process + 2));	 // main diag
	b_ver = malloc(sizeof(double) * (size_per_process + 2)); // main diag

	c = malloc(sizeof(double) * (size_per_process + 2));	 // sub diag
	c_ver = malloc(sizeof(double) * (size_per_process + 2)); // sub diag

	r = malloc(sizeof(double) * (size_per_process + 2));	 // rhs
	r_ver = malloc(sizeof(double) * (size_per_process + 2)); // rhs

	x = malloc(sizeof(double) * (size_per_process + 2)); // result array

	for (int i = 0; i < size_per_process + 2; i++) {
		a[i] = 0.0;
		b[i] = 1.0;
		c[i] = 0.0;
		x[i] = 0.0;
		r[i] = 0.0;
	}
	for (int i = 1; i < size_per_process + 1; i++) {
		a[i] = -1.0;
		b[i] = 2.0;
		c[i] = -1.0;
		r[i] = ((double)rand()) / RAND_MAX;
		// r[i] = i * mynode;
		a_ver[i] = -1.0;
		b_ver[i] = 2.0;
		c_ver[i] = -1.0;
		r_ver[i] = r[i];
	}
	start_time = MPI_Wtime();
	forward_multi(a, b, c, r, size_per_process, nprocs, mynode);
	forward_single(a, b, c, x, r, size_per_process, nprocs, mynode);
	backsubstitution(a, b, c, x, r, size_per_process, nprocs, mynode);
	elapsed_time = MPI_Wtime() - start_time;

	MPI_Barrier(MPI_COMM_WORLD);
	double *solution = NULL;
	double *rhs = NULL;
	if (mynode == 0) {
		solution = malloc(sizeof(double) * matrix_size);
		rhs = malloc(sizeof(double) * matrix_size);
	}
	MPI_Gather(x + 1, size_per_process, MPI_DOUBLE, solution, size_per_process, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Gather(r_ver + 1, size_per_process, MPI_DOUBLE, rhs, size_per_process, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	if (mynode == 0) {
		// printf("Complete Solution:\n");
		// for (int i = 0; i < matrix_size; i++) {
		// 	printf("%.5f ", solution[i]);
		// }
		// printf("yarrak\n");
		// for (int i = 0; i < matrix_size; i++) {
		// 	printf("%.5f ", rhs[i]);
		// }
		// printf("\n");
		write_results(solution, rhs, matrix_size, &elapsed_time);
	}
	// free(y_ver);

	free(a);
	free(b);
	free(c);
	free(x);
	free(r);
	free(a_ver);
	free(b_ver);
	free(c_ver);
	free(r_ver);

	MPI_Finalize();

	return 0;
}

void write_results(double *solution, double *rhs, int matrix_size, double *time) {
	struct stat st = {0};

	// Check if directory exists; if not, create it
	if (stat("parallel_solutions", &st) == -1) {
		if (mkdir("parallel_solutions", 0700) != 0) {
			perror("Failed to create directory");
			return;
		}
	}

	char file_path[64] = {0};
	snprintf(file_path, sizeof(file_path), "parallel_solutions/%d", matrix_size);

	FILE *file = fopen(file_path, "w");
	if (!file) {
		perror("Error opening solution file");
		return;
	}
	fwrite(solution, sizeof(double), matrix_size, file);
	fwrite(rhs, sizeof(double), matrix_size, file);
	fwrite(time, sizeof(double), 1, file);

	fclose(file);
	printf("Solution written to %s\n", file_path);
}
void forward_multi(double *a, double *b, double *c, double *r, int size_per_process, int nprocs, int mynode) {
	int i, l;
	int nlevel;
	int ip, in, start, dist_row, dist2_row;
	double alpha, gamma;
	double sbuf[4], rbuf[4];

	MPI_Status status, status1;
	MPI_Request request[2];

	/// Variable nlevel is used to indicates when single row remains.
	nlevel = log2(size_per_process);
	dist_row = 1;
	dist2_row = 2;

	for (l = 0; l < nlevel; l++) {
		start = dist2_row;
		if (mynode < nprocs - 1) {
			MPI_Irecv(rbuf, 4, MPI_DOUBLE, mynode + 1, 0, MPI_COMM_WORLD, request);
		}
		if (mynode > 0) {
			sbuf[0] = a[dist_row];
			sbuf[1] = b[dist_row];
			sbuf[2] = c[dist_row];
			sbuf[3] = r[dist_row];
			MPI_Isend(sbuf, 4, MPI_DOUBLE, mynode - 1, 0, MPI_COMM_WORLD, request + 1);
		}
		if (mynode < nprocs - 1) {
			MPI_Wait(request, &status1);
			a[size_per_process + 1] = rbuf[0];
			b[size_per_process + 1] = rbuf[1];
			c[size_per_process + 1] = rbuf[2];
			r[size_per_process + 1] = rbuf[3];
		}

		for (i = start; i <= size_per_process; i += dist2_row) {
			ip = i - dist_row;
			in = i + dist_row < size_per_process + 1 ? i + dist_row : size_per_process + 1;
			alpha = -a[i] / b[ip];
			gamma = -c[i] / b[in];

			b[i] += (alpha * c[ip] + gamma * a[in]);
			a[i] = alpha * a[ip];
			c[i] = gamma * c[in];
			r[i] += (alpha * r[ip] + gamma * r[in]);
		}
		dist2_row *= 2;
		dist_row *= 2;

		if (mynode > 0) {
			MPI_Wait(request + 1, &status);
		}
	}
}
void forward_single(double *a, double *b, double *c, double *x, double *r, int size_per_process, int nprocs, int mynode) {
	int i, l, nhprocs;
	int nlevel;
	int ip, in, dist_rank;
	int mynode_level, nprocs_level;
	double alpha, gamma, det;
	double sbuf[4], rbuf0[4], rbuf1[4];

	MPI_Status status;
	MPI_Request request[4];

	nlevel = log2(nprocs);
	nhprocs = nprocs / 2;
	dist_rank = 1;

	for (l = 0; l < nlevel - 1; l++) {

		mynode_level = mynode / dist_rank;
		nprocs_level = nprocs / dist_rank;

		sbuf[0] = a[size_per_process];
		sbuf[1] = b[size_per_process];
		sbuf[2] = c[size_per_process];
		sbuf[3] = r[size_per_process];

		if ((mynode_level + 1) % 2 == 0) {
			if (mynode + dist_rank < nprocs) {
				MPI_Irecv(rbuf1, 4, MPI_DOUBLE, mynode + dist_rank, 202, MPI_COMM_WORLD, request);
				MPI_Isend(sbuf, 4, MPI_DOUBLE, mynode + dist_rank, 203, MPI_COMM_WORLD, request + 1);
			}
			if (mynode - dist_rank >= 0) {
				MPI_Irecv(rbuf0, 4, MPI_DOUBLE, mynode - dist_rank, 200, MPI_COMM_WORLD, request + 2);
				MPI_Isend(sbuf, 4, MPI_DOUBLE, mynode - dist_rank, 201, MPI_COMM_WORLD, request + 3);
			}
			if (mynode + dist_rank < nprocs) {
				MPI_Wait(request, &status);
				a[size_per_process + 1] = rbuf1[0];
				b[size_per_process + 1] = rbuf1[1];
				c[size_per_process + 1] = rbuf1[2];
				r[size_per_process + 1] = rbuf1[3];
				MPI_Wait(request + 1, &status);
			}
			if (mynode - dist_rank >= 0) {
				MPI_Wait(request + 2, &status);
				a[0] = rbuf0[0];
				b[0] = rbuf0[1];
				c[0] = rbuf0[2];
				r[0] = rbuf0[3];
				MPI_Wait(request + 3, &status);
			}
		} else if ((mynode_level + 1) % 2 == 1) {
			if (mynode + dist_rank < nprocs) {
				MPI_Irecv(rbuf1, 4, MPI_DOUBLE, mynode + dist_rank, 201, MPI_COMM_WORLD, request);
				MPI_Isend(sbuf, 4, MPI_DOUBLE, mynode + dist_rank, 200, MPI_COMM_WORLD, request + 1);
			}
			if (mynode - dist_rank >= 0) {
				MPI_Irecv(rbuf0, 4, MPI_DOUBLE, mynode - dist_rank, 203, MPI_COMM_WORLD, request + 2);
				MPI_Isend(sbuf, 4, MPI_DOUBLE, mynode - dist_rank, 202, MPI_COMM_WORLD, request + 3);
			}
			if (mynode + dist_rank < nprocs) {
				MPI_Wait(request, &status);
				a[size_per_process + 1] = rbuf1[0];
				b[size_per_process + 1] = rbuf1[1];
				c[size_per_process + 1] = rbuf1[2];
				r[size_per_process + 1] = rbuf1[3];
				MPI_Wait(request + 1, &status);
			}
			if (mynode - dist_rank >= 0) {
				MPI_Wait(request + 2, &status);
				a[0] = rbuf0[0];
				b[0] = rbuf0[1];
				c[0] = rbuf0[2];
				r[0] = rbuf0[3];
				MPI_Wait(request + 3, &status);
			}
		}

		i = size_per_process;
		ip = 0;
		in = i + 1;
		if (mynode_level == 0) {
			alpha = 0.0;
		} else {
			alpha = -a[i] / b[ip];
		}
		if (mynode_level == nprocs_level - 1) {
			gamma = 0.0;
		} else {
			gamma = -c[i] / b[in];
		}

		b[i] += (alpha * c[ip] + gamma * a[in]);
		a[i] = alpha * a[ip];
		c[i] = gamma * c[in];
		r[i] += (alpha * r[ip] + gamma * r[in]);

		dist_rank *= 2;
	}

	sbuf[0] = a[size_per_process];
	sbuf[1] = b[size_per_process];
	sbuf[2] = c[size_per_process];
	sbuf[3] = r[size_per_process];
	if (mynode < nhprocs) {
		MPI_Irecv(rbuf1, 4, MPI_DOUBLE, mynode + nhprocs, 300, MPI_COMM_WORLD, request);
		MPI_Isend(sbuf, 4, MPI_DOUBLE, mynode + nhprocs, 301, MPI_COMM_WORLD, request + 1);

		MPI_Wait(request, &status);
		a[size_per_process + 1] = rbuf1[0];
		b[size_per_process + 1] = rbuf1[1];
		c[size_per_process + 1] = rbuf1[2];
		r[size_per_process + 1] = rbuf1[3];

		i = size_per_process;
		in = size_per_process + 1;

		det = b[i] * b[in] - c[i] * a[in];
		x[i] = (r[i] * b[in] - r[in] * c[i]) / det;
		x[in] = (r[in] * b[i] - r[i] * a[in]) / det;
		MPI_Wait(request + 1, &status);

	} else if (mynode >= nhprocs) {
		MPI_Irecv(rbuf0, 4, MPI_DOUBLE, mynode - nhprocs, 301, MPI_COMM_WORLD, request + 2);
		MPI_Isend(sbuf, 4, MPI_DOUBLE, mynode - nhprocs, 300, MPI_COMM_WORLD, request + 3);

		MPI_Wait(request + 2, &status);
		a[0] = rbuf0[0];
		b[0] = rbuf0[1];
		c[0] = rbuf0[2];
		r[0] = rbuf0[3];

		ip = 0;
		i = size_per_process;

		det = b[ip] * b[i] - c[ip] * a[i];
		x[ip] = (r[ip] * b[i] - r[i] * c[ip]) / det;
		x[i] = (r[i] * b[ip] - r[ip] * a[i]) / det;
		MPI_Wait(request + 3, &status);
	}
}

void backsubstitution(double *a, double *b, double *c, double *x, double *r, int size_per_process, int nprocs, int mynode) {
	int i, l;
	int nlevel;
	int ip, in, dist_row, dist2_row;

	MPI_Status status;
	MPI_Request request[2];

	nlevel = log2(size_per_process);
	dist_row = size_per_process / 2;

	if (mynode > 0) {
		MPI_Irecv(x, 1, MPI_DOUBLE, mynode - 1, 100, MPI_COMM_WORLD, request);
	}
	if (mynode < nprocs - 1) {
		MPI_Isend(x + size_per_process, 1, MPI_DOUBLE, mynode + 1, 100, MPI_COMM_WORLD, request + 1);
	}
	if (mynode > 0) {
		MPI_Wait(request, &status);
	}
	for (l = nlevel - 1; l >= 0; l--) {
		dist2_row = dist_row * 2;
		for (i = size_per_process - dist_row; i >= 0; i -= dist2_row) {
			ip = i - dist_row;
			in = i + dist_row;
			x[i] = r[i] - c[i] * x[in] - a[i] * x[ip];
			x[i] = x[i] / b[i];
		}
		dist_row = dist_row / 2;
	}
	if (mynode < nprocs - 1) {
		MPI_Wait(request + 1, &status);
	}
}
