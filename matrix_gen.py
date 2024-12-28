import os
import sys
import random
import numpy as np


num_matrices = 20
matrix_folder = './matrixes/'
sizes = [5, 10, 20, 30, 40, 50, 100, 200, 400, 800, 1600]
for i in sizes:
    file_name = matrix_folder + str(i) + 'x' + str(i)
    print(f"generating {file_name}")
    # matrix = np.random.rand(i+1, i)*100
    # b = matrix[i]
    # matrix = matrix[:i]
    main_diag = np.random.uniform(0, 100, i)
    upper_diag = np.random.uniform(0, 100, i - 1)
    lower_diag = np.random.uniform(0, 100, i - 1)
    matrix = np.diag(main_diag) + np.diag(upper_diag, k=1) + \
        np.diag(lower_diag, k=-1)
    b = np.random.uniform(0, 100, i)
    print(matrix)
    print(b)
    print(np.linalg.solve(matrix, b))
    matrix = np.vstack((matrix, b))
    matrix.tofile(file_name)
    # np.savetxt(file_name, matrix, delimiter=',')
