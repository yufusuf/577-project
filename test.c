#include "cyclic.h"
#include "linear_solver.h"
#include "tridiagonal_matrix.h"
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define EPSILON 0.000001
#define abs(x) ((x) < 0 ? -(x) : (x))

const char *TEST_PASSED = "\033[32m\tTEST PASSED\033[0m";
const char *TEST_FAILED = "\033[31m\tTEST FAILED\033[0m";
double calculate_l2_norm(double *vector, int size);
void print_matrix(double *A, int size);
void print_vect(double *b, int size);
int check_result(double *true_solution, double *solution, int dim, int verbose);
double calculate_residual_error(struct tridiagonal_matrix *matrix, double *solution, double *b, int matrix_size);
double calculate_l2_norm(double *vector, int size);
void extract_diagonals(double *A, int size, double *dl, double *d, double *du);
double get_time();

void print_matrix(double *A, int size) {
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			printf("%f ", A[i * size + j]);
		}
		printf("\n");
	}
}
void print_vect(double *b, int size) {
	for (int j = 0; j < size; j++) {
		printf("%f ", b[j]);
	}
	printf("\n");
}
// dont use, calculate correctness with residual_error
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
double calculate_residual_error(struct tridiagonal_matrix *matrix, double *solution, double *b, int matrix_size) {
	double Ax = 0.0;
	double residual = 0.0;

	// Calculate Ax
	for (int i = 0; i < matrix_size; i++) {
		Ax = 0.0;
		if (i > 0) {
			Ax += matrix->dl[i] * solution[i - 1];
		}
		Ax += matrix->d[i] * solution[i];
		if (i < matrix_size - 1) {
			Ax += matrix->du[i] * solution[i + 1];
		}
		residual += (Ax - b[i]) * (Ax - b[i]);
	}

	return sqrt(residual);
}
double calculate_l2_norm(double *vector, int size) {
	double norm = 0.0;
	for (int i = 0; i < size; i++) {
		norm += vector[i] * vector[i];
	}
	return sqrt(norm);
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
	struct tridiagonal_matrix *A = NULL;
	double *b;
	int matrix_size;
	DIR *dir;
	FILE *matrix_f;
	char *matrix_path;
	char *p;
	double time;
	double err;
	double *result;

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
		A = init_tmatrix(matrix_size);
		alloc_tmatrix(A);
		b = malloc(matrix_size * sizeof(double)); // rhs

		fread(A->d, sizeof(double), matrix_size, matrix_f);
		fread(A->du, sizeof(double), matrix_size, matrix_f);
		fread(A->dl, sizeof(double), matrix_size, matrix_f);
		fread(b, sizeof(double), matrix_size, matrix_f);

		result = malloc(sizeof(double) * matrix_size);

		// linear solver (DGESV) ==============
		// time = get_time();
		// linear_system_solver(A, b, result);
		// time = get_time() - time;
		// err = calculate_residual_error(A, result, b, matrix_size);
		//
		// printf("DGESV (linear solver): \tTIME: \t %.2f ms \t residual_err: %e\n", time * 1000, err);
		//
		// // ==============
		//
		// // tridiag solver DGTSV ==============
		// time = get_time();
		// tridiag_system_solver(A, b, result);
		// time = get_time() - time;
		// err = calculate_residual_error(A, result, b, matrix_size);
		//
		// printf("DGTSV (tridia solver): \tTIME: \t %.2f ms \t residual err: %e\n", 1000 * time, err);
		// ================

		// sequential crc ==============
		time = get_time();
		cyclic_reduction_seq(A, b, result);
		time = get_time() - time;
		err = calculate_residual_error(A, result, b, matrix_size);

		printf("SEQ: %s, \tTIME: \t %.2f ms \t residual err: %e\n", err < EPSILON ? TEST_PASSED : TEST_FAILED, time * 1000, err);
		// ==============

		// parallel ==============
		// memset(result, 0, sizeof(double) * matrix_size);
		// memcpy(temp_A, A, matrix_size * matrix_size * sizeof(double));
		//
		// time = get_time();
		// info = cyclic_reduction_parallel(temp_A, matrix_size, b, result);
		// time = get_time() - time;
		// err = calculate_residual_error(A, result, b, matrix_size);
		//
		// printf("PARA: %s, \tTIME: \t %.2f ms \t residual err: %e \n", err < EPSILON ? TEST_PASSED : TEST_FAILED, 1000 * time, err);
		//==============
		printf("\n");

		free(matrix_path);
		free(p);
		free(result);
		free(b);

		fclose(matrix_f);
		free_tmatrix(A);
	}
	closedir(dir);
	return 0;
}
