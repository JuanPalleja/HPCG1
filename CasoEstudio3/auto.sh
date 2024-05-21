#!/bin/bash

# Specify the output file
output_file="resultado_mpi.csv"

> $output_file

# Loop to run the program multiple times for each matrix size
for matrix_size in 1000 1500 2000 2500 3000 3500 4000
do
    echo "Running with matrix size: $matrix_size"
    
    # Run the program 10 times
    for run in {1..5}
    do
        echo "   Run $run"
        mpiexec -hosts head,wn1,wn2,wn3 ./exe $matrix_size >> $output_file
    done
done

echo "All executions completed. Results saved in $output_file"