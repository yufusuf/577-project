
#include "linear_solver.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
int main() {
	// double A[] = {2, -1, 0, 1, -1, 2, -1, 0, 0, -1, 2, 1, 1, 1, 1, 1};
	// double b[] = {1, 3, 5, 3};
	// int n = 4;
	// int info;
	// double result[n];
	//
	// info = linear_system_solver(A, n, b, result);
	//
	// if (info > 0) {
	// 	printf("The matrix is singular; the solution could not be "
	// 	       "computed.\n");
	// 	return EXIT_FAILURE;
	// }
	//
	// // Print the solution
	// printf("Solution:\n");
	// for (int i = 0; i < n; i++) {
	// 	printf("x[%d] = %f\n", i, result[i]);
	// }
	//
	// return EXIT_SUCCESS;
	const char *path = "./matrixes/";
	struct dirent *entry;
	int matrix_size;
	DIR *dir;
	FILE *matrix_f;
	char *matrix_path;
	char *p;
	double *matrix;
	double *b;

	dir = opendir(path);
	if (!dir) {
		perror("unable to open matrixes");
		return 1;
	}
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, "..") == 0 ||
		    strcmp(entry->d_name, ".") == 0)
			continue;
		p = strdup(entry->d_name);
		matrix_size = atoi(strtok(p, "x"));
		matrix_path = malloc(
		    sizeof(char) * (strlen(path) + strlen(entry->d_name) + 1));
		strncpy(matrix_path, path, strlen(path));
		strcat(matrix_path, entry->d_name);
		printf("Running %d, %s\n", matrix_size, matrix_path);
		matrix_f = fopen(matrix_path, "rb");
		if (!matrix_f) {
			perror("unable to open matrix file");
			printf("filepath: %s\n", matrix_path);
			return 1;
		}
		matrix = malloc(matrix_size * matrix_size * sizeof(double));
		b = malloc(matrix_size * sizeof(double));

		fread(matrix, sizeof(double), matrix_size * matrix_size,
		      matrix_f);
		fread(b, sizeof(double), matrix_size, matrix_f);
		for (int i = 0; i < matrix_size; i++) {
			for (int j = 0; j < matrix_size; j++) {
				printf("%f ", matrix[i * matrix_size + j]);
			}
			printf("\n");
		}
		for (int i = 0; i < matrix_size; i++) {
			printf("%f ", b[i]);
		}
		printf("\n");

		// double result[matrix_size];
		// int info = linear_system_solver(A, n, b, result);

		// if (info > 0) {
		// 	printf("The matrix is singular; the solution could not
		// be " 	       "computed.\n"); 	return EXIT_FAILURE;
		// }
		//
		// // Print the solution
		// printf("Solution:\n");
		// for (int i = 0; i < n; i++) {
		// 	printf("x[%d] = %f\n", i, result[i]);
		// }

		free(matrix_path);
		fclose(matrix_f);
		free(p);
		free(matrix);
		break;
	}
	closedir(dir);
	return 0;
}
