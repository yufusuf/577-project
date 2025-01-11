#include "aux.h"
#include "cyclic_serial.h"
#include "linear_solver.h"
#include "tridiagonal_matrix.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
	const char *matrix_dir_path = "./matrixes/";
	struct dirent *entry;
	struct tridiagonal_matrix *A = NULL;
	double *b;
	struct tridiagonal_matrix *p_matrix;
	double *p_result;
	double *p_b;
	int matrix_size;
	DIR *dir;
	FILE *matrix_f;
	char *matrix_path = NULL;
	char *p;
	double time;
	double err;
	double *result;

	dir = opendir(matrix_dir_path);
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
		matrix_path = calloc((strlen(matrix_dir_path) + strlen(entry->d_name) + 1), sizeof(char));
		strncpy(matrix_path, matrix_dir_path, strlen(matrix_dir_path));
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
		time = tridiag_system_solver(A, b, result);
		err = calculate_residual_error(A, result, b, matrix_size);
		printf("DGTSV (tridia solver)	      :\t\t\t TIME: %.6f ms \t residual err: %-10e\n", 1000 * time, err);
		// ================

		// sequential crc low mem==============
		time = cyclic_reduction_seq_low_mem(A, b, result);
		err = calculate_residual_error(A, result, b, matrix_size);
		printf("SEQ_l: (serial cyclic solver):\t\t\t TIME: " RED_TEXT("%.6f ms") "\t residual err: %-10e\n", time * 1000, err);
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

		// parallel ============== we read results from parallel_solutions folder
		// read parallel results into p_matrix and p_result arrays
		p_matrix = init_tmatrix(matrix_size + 1);
		alloc_tmatrix(p_matrix);
		p_b = malloc((matrix_size + 1) * sizeof(double)); // rhs
		p_result = malloc((matrix_size + 1) * sizeof(double));
		if (read_parallel_results(p_matrix, p_b, p_result, matrix_size + 1, &time) == 0) {
			err = calculate_residual_error(p_matrix, p_result, p_b, matrix_size + 1);
			printf("PARA: (parallel cyclic solver):\t\t\t TIME: " GREEN_TEXT("%.6f ms") "\t residual err: %10e \n", 1000 * time, err);
		} else {
			printf("PARA: no solution for %d\n", matrix_size + 1);
		}
		//==============
		printf("\n");

		free(matrix_path);
		free(p);
		free(result);
		free(p_result);
		free(b);
		free(p_b);

		fclose(matrix_f);
		free_tmatrix(A);
		free_tmatrix(p_matrix);
	}
	closedir(dir);
	return 0;
}
