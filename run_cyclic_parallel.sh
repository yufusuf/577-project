#!/bin/bash

for arg in {7..24}; do
    shifted=$((1<<arg)) 
    echo "Running cyclic_parallel with $shifted..."
    sbatch -o slurm_outs/$shifted.out slurm_test.sh $shifted
    echo ""
done

