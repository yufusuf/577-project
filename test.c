#include "cyclic.h"
#include "linear_solver.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define EPSILON 0.00001
#define abs(x) ((x) < 0 ? -(x) : (x))

const char *TEST_PASSED = "\033[32m\tTEST PASSED\033[0m";
const char *TEST_FAILED = "\033[31m\tTEST FAILED\033[0m";

void print_matrix_1(double *A, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			printf("%f ", A[i * size + j]);
		}
		printf("\n");
	}
}
int check_result(double *true_solution, double *solution, int dim, int verbose) {
	int correct = 1;
	for (int i = 0; i < dim; i++) {
		// test failed
		if (abs(solution[i] - true_solution[i]) > EPSILON) {
			correct = 0;
			if (verbose)
				printf("\t difference larger than epsilon: "
				       "true[%d]=%f, ours[%d]=%f, dif=%f\n",
				       i, true_solution[i], i, solution[i], abs(solution[i] - true_solution[i]));
		}
	}
	return correct;
}

void extract_diagonals(double *A, int size, double *dl, double *d, double *du) {
	for (int i = 0; i < size; i++) {
		d[i] = A[index_of(i, size, i)];
		if (i < size - 1)
			du[i] = A[index_of(i, size, i + 1)];
		if (i > 0)
			dl[i - 1] = A[index_of(i, size, i - 1)];
	}
}
double get_time() {
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	return now.tv_sec + now.tv_nsec * 1e-9;
}

int main() {
	const char *path = "./matrixes/";
	struct dirent *entry;
	int matrix_size;
	DIR *dir;
	FILE *matrix_f;
	char *matrix_path;
	char *p;
	double *matrix;
	double *b;
	double *temp_A;
	int test_result;
	double time;

	dir = opendir(path);
	if (!dir) {
		perror("unable to open matrixes");
		return 1;
	}
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".") == 0)
			continue;
		// read test file name
		p = strdup(entry->d_name);
		matrix_size = atoi(strtok(p, "x"));
		matrix_path = malloc(sizeof(char) * (strlen(path) + strlen(entry->d_name) + 1));
		strncpy(matrix_path, path, strlen(path));
		strcat(matrix_path, entry->d_name);

		printf("RUNNING TEST ON: %s \n", matrix_path);

		// open test file
		matrix_f = fopen(matrix_path, "rb");
		if (!matrix_f) {
			perror("unable to open matrix file");
			printf("filepath: %s\n", matrix_path);
			return 1;
		}
		// read Ax=b, A and b
		matrix = malloc(matrix_size * matrix_size * sizeof(double));
		b = malloc(matrix_size * sizeof(double));

		fread(matrix, sizeof(double), matrix_size * matrix_size, matrix_f);
		fread(b, sizeof(double), matrix_size, matrix_f);
		double true_result[matrix_size];
		double cyclic_result[matrix_size];

		temp_A = malloc(sizeof(double) * matrix_size * matrix_size);

		// copy matrixes as LAPACKE modifies original input
		memset(cyclic_result, 0, sizeof(double) * matrix_size);
		memcpy(temp_A, matrix, matrix_size * matrix_size * sizeof(double));

		// linear solver (DGESV) ==============
		time = get_time();
		int info = linear_system_solver(temp_A, matrix_size, b, true_result);
		time = get_time() - time;

		printf("DGESV (linear solver): \tTIME: \t %f ms\n", time * 1000);

		if (info > 0) {
			printf("linear_system_solver: The matrix is singular; "
			       "the solution could "
			       "notbe computed.\n");
			return EXIT_FAILURE;
		}
		// ==============

		// tridiag solver DGTSV ==============
		memcpy(temp_A, matrix, matrix_size * matrix_size * sizeof(double));

		double *du = malloc(sizeof(double) * (matrix_size - 1));
		double *dl = malloc(sizeof(double) * (matrix_size - 1));
		double *d = malloc(sizeof(double) * (matrix_size));
		extract_diagonals(matrix, matrix_size, dl, d, du);

		time = get_time();
		info = tridiag_system_solver(dl, d, du, b, matrix_size, cyclic_result);
		time = get_time() - time;

		printf("DGTSV (tridia solver): \tTIME: \t %f ms\n", 1000 * time);
		// ================

		// sequential crc ==============
		memcpy(temp_A, matrix, matrix_size * matrix_size * sizeof(double));

		time = get_time();
		info = cyclic_reduction_seq(temp_A, matrix_size, b, cyclic_result);
		time = get_time() - time;

		if (info > 0) {
			printf(" cyclic_reduction: could not compute");
		}

		test_result = check_result(true_result, cyclic_result, matrix_size, 0);

		printf("SEQ: %s, \tTIME: \t %f ms\n", test_result ? TEST_PASSED : TEST_FAILED, time * 1000);
		// ==============

		// parallel ==============
		memset(cyclic_result, 0, sizeof(double) * matrix_size);
		memcpy(temp_A, matrix, matrix_size * matrix_size * sizeof(double));

		time = get_time();
		info = cyclic_reduction_parallel(temp_A, matrix_size, b, cyclic_result);
		time = get_time() - time;

		test_result = check_result(true_result, cyclic_result, matrix_size, 0);

		printf("PARA: %s, \tTIME: \t %f ms\n", test_result ? TEST_PASSED : TEST_FAILED, 1000 * time);
		//==============
		printf("\n");

		free(matrix_path);
		fclose(matrix_f);
		free(p);
		free(dl);
		free(du);
		free(d);
		free(matrix);
		free(temp_A);
		free(b);
	}
	closedir(dir);
	return 0;
}
