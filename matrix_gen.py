# import os
# import sys
# import random
import numpy as np


def random_gen(size):
    main_diag = np.random.uniform(0, 1000, size)
    upper_diag = np.random.uniform(0, 1000, size)
    lower_diag = np.random.uniform(0, 1000, size)
    b = np.random.uniform(0, 1000, size)
    lower_diag[0] = 0
    upper_diag[size - 1] = 0
    return main_diag, upper_diag, lower_diag, b


def sikko_gen(size):
    main_diag = np.full(size, 2).astype(np.float64)
    upper_diag = np.full(size, -1).astype(np.float64)
    lower_diag = np.full(size, -1).astype(np.float64)
    lower_diag[0] = 0
    upper_diag[size - 1] = 0
    b = np.full(size, 1).astype(np.float64)
    return main_diag, upper_diag, lower_diag, b


num_matrices = int(input("Enter N:"))
matrix_folder = './matrixes/'
for i in range(3, num_matrices):
    matrix_size = 2**i - 1
    file_name = matrix_folder + str(matrix_size) + 'x' + str(matrix_size)
    print(f"generating {file_name}")
    main_diag, upper_diag, lower_diag, b = sikko_gen(matrix_size)
    # matrix = np.diag(main_diag) + np.diag(upper_diag[:matrix_size - 1], k=1) + np.diag(lower_diag[1:], k=-1)
    # print(np.linalg.solve(matrix, b))

    matrix = np.hstack((main_diag, upper_diag, lower_diag, b))
    matrix.tofile(file_name)
