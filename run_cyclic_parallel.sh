#!/bin/bash

for arg in {4..24}; do
    shifted=$((1<<arg)) 
    echo "Running cyclic_parallel with $shifted..."
    mpirun -np 4 cyclic_parallel $shifted
    echo ""
done

