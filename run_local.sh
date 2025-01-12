#!/bin/bash

# # Check if exactly two arguments are supplied
# if [ "$#" -ne 2 ]; then
#   echo "Usage: $0 <arg1> <arg2>"
#   echo "give sizes in log2"
#   exit 1
# fi
#
FROM=$1
TO=$2
#
# if [ "$FROM" -lt 7 ]; then
# 	echo "starting size cannot be smaller than 7"
# 	exit 1
# fi
# if [ "$TO" -gt 25 ]; then
# 	echo "end size cannot be larger than 25"
# 	exit 1
# fi
#
# echo "Generating matrices..."
# mkdir -p matrices 
#
# python3 matrix_gen.py "$FROM" "$((TO + 1))"

for arg in $(seq "$FROM" "$TO"); do
    # Compute the shifted value
    size=$((1 << arg))

    echo "Running cyclic_parallel with size: $size"

    mpirun -np 4 cyclic_parallel $size

done

./test

echo "generating results and plots in results directory..."
mkdir -p results

python3 plot_generator.py



