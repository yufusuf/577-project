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
    main_diag = np.full(matrix_size, 2).astype(np.float64)
    upper_diag = np.full(matrix_size, -1).astype(np.float64)
    lower_diag = np.full(matrix_size, -1).astype(np.float64)
    upper_diag[matrix_size - 1] = 0.0
    lower_diag[0] = 0.0
    # matrix = np.diag(main_diag) + np.diag(upper_diag, k=1) + \
    #     np.diag(lower_diag, k=-1)
    # b = np.random.uniform(0, 1, matrix_size)
    b = np.full(matrix_size, 1).astype(np.float64)
    # print(matrix)
    # print(b)
    # try:
    #     res = np.linalg.solve(matrix, b)
    # except np.LinAlgError:
    #     print(f"no solution to matrix on {matrix_size}")
    matrix = np.hstack((main_diag, upper_diag, lower_diag, b))
    # print(matrix)
    matrix.tofile(file_name)
    # np.savetxt(file_name, matrix, delimiter=',')

# for other testing
# main_diag = np.full((matrix_size,), -2).astype(np.float64)
# upper_diag = np.full((matrix_size - 1), 1).astype(np.float64)
# lower_diag = np.full((matrix_size - 1), 1).astype(np.float64)
# matrix = np.diag(main_diag) + np.diag(upper_diag, k=1) + np.diag(lower_diag, k=-1)
# b = np.arange(matrix_size).astype(np.float64)
