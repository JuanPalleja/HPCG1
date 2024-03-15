#!/bin/bash

# Compile the C program
gcc -O2 matrices_secuencial_opt_cache.c -o test

# Specify the output file
output_file="resultado_matrices_secuencial_opt_cache.txt"

> $output_file

# Loop to run the program multiple times for each matrix size
for matrix_size in 500 1000 1500 2000 2500 3000
do
    echo "Running with matrix size: $matrix_size"
    
    # Run the program 10 times
    for run in {1..10}
    do
        echo "   Run $run"
        ./test $matrix_size >> $output_file
    done
done

echo "All executions completed. Results saved in $output_file"