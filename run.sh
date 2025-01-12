#!/bin/bash

# Check if exactly two arguments are supplied
if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <arg1> <arg2>"
  echo "give sizes in log2"
  exit 1
fi

FROM=$1
TO=$2

if [ "$FROM" -lt 7 ]; then
	echo "starting size cannot be smaller than 7"
	exit 1
fi
if [ "$TO" -gt 25 ]; then
	echo "end size cannot be larger than 25"
	exit 1
fi

echo "Generating matrices..."
mkdir -p matrices 

python3 matrix_gen.py "$FROM" "$((TO + 1))"

./run_cyclic_parallel.sh "$FROM" "$TO"

echo "No jobs matching $USER in the queue. Proceeding to run tests..."
	./test

echo "generating results and plots in results directory..."
mkdir -p results

python3 plot_generator.py



