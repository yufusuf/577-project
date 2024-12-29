# import os
# import sys
# import random
import numpy as np


num_matrices = 20
matrix_folder = './matrixes/'
for i in range(3, 15):
    matrix_size = 2**i - 1
    file_name = matrix_folder + str(matrix_size) + 'x' + str(matrix_size)
    print(f"generating {file_name}")
    # matrix = np.random.rand(i+1, i)*100
    # b = matrix[i]
    # matrix = matrix[:i]
    main_diag = np.random.uniform(0, 100, matrix_size)
    upper_diag = np.random.uniform(0, 100, matrix_size - 1)
    lower_diag = np.random.uniform(0, 100, matrix_size - 1)
    matrix = np.diag(main_diag) + np.diag(upper_diag, k=1) + \
        np.diag(lower_diag, k=-1)
    b = np.random.uniform(0, 100, matrix_size)
    print(matrix)
    print(b)
    try:
        res = np.linalg.solve(matrix, b)
    except np.LinAlgError:
        print(f"no solution to matrix on {matrix_size}")
    matrix = np.vstack((matrix, b))
    matrix.tofile(file_name)
    # np.savetxt(file_name, matrix, delimiter=',')

# for other testing
# main_diag = np.full((matrix_size,), -2).astype(np.float64)
# upper_diag = np.full((matrix_size - 1), 1).astype(np.float64)
# lower_diag = np.full((matrix_size - 1), 1).astype(np.float64)
# matrix = np.diag(main_diag) + np.diag(upper_diag, k=1) + np.diag(lower_diag, k=-1)
# b = np.arange(matrix_size).astype(np.float64)
