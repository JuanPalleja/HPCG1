#!/bin/bash

# Compile the C program
gcc -O3 optimizar.c -o testO3

# Specify the output file
output_file="optimizarO3.txt"

> $output_file

# Loop to run the program multiple times for each matrix size
for matrix_size in 500 1000 1500
do
    echo "Running with matrix size: $matrix_size"
    
    # Run the program 5 times
    for run in {1..5}
    do
        echo "   Run $run"
        ./testO3 $matrix_size >> $output_file
    done
done

echo "All executions completed. Results saved in $output_file"