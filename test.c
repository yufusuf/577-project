#include "aux.h"
#include "cyclic_serial.h"
#include "linear_solver.h"
#include "tridiagonal_matrix.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *TEST_PASSED = "\033[32m\tTEST PASSED\033[0m";
const char *TEST_FAILED = "\033[31m\tTEST FAILED\033[0m";

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

		//
		// tridiag solver DGTSV ==============
		time = get_time();
		tridiag_system_solver(A, b, result);
		time = get_time() - time;
		err = calculate_residual_error(A, result, b, matrix_size);

		printf("DGTSV (tridia solver): \tTIME: \t %.2f ms \t residual err: %e\n", 1000 * time, err);
		// ================

		// sequential crc low mem==============
		time = get_time();
		cyclic_reduction_seq_low_mem(A, b, result);
		time = get_time() - time;
		err = calculate_residual_error(A, result, b, matrix_size);

		printf("SEQ_l: %s, \tTIME: \t %.2f ms \t residual err: %e\n", err < EPSILON ? TEST_PASSED : TEST_FAILED, time * 1000, err);
		// ==============

		if (matrix_size < (1 << 14)) {
			// // linear solver (DGESV) ==============
			// time = get_time();
			// linear_system_solver(A, b, result);
			// time = get_time() - time;
			// err = calculate_residual_error(A, result, b, matrix_size);
			//
			// printf("DGESV (linear solver): \tTIME: \t %.2f ms \t residual_err: %e\n", time * 1000, err);
			// // ==============
			//
			// // sequential crc high mem==============
			// time = get_time();
			// cyclic_reduction_seq_high_mem(A, b, result);
			// time = get_time() - time;
			// err = calculate_residual_error(A, result, b, matrix_size);
			//
			// printf("SEQ_h: %s, \tTIME: \t %.2f ms \t residual err: %e\n", err < EPSILON ? TEST_PASSED : TEST_FAILED, time * 1000, err);
			// ==============
		}

		// parallel ============== do execv here
		// time = get_time();
		// cyclic_reduction_parallel(A, b, result);
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
