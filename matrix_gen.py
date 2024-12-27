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
    matrix = np.random.rand(i+1, i)*100
    print(matrix)
    # np.savetxt(file_name, matrix, delimiter=',')
    matrix.tofile(file_name)
    break
