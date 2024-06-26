#!/bin/bash

# Compile the C program
 gcc -o test matrices_OpenMp.c -fopenmp

# Specify the output file
output_file="resultado_OpenMp.csv"

> $output_file

# Loop to run the program multiple times for each matrix size
for num_threads in 1 2 4 8 16 32 64
do
    for matrix_size in 500 1000 1500 2000 2500 3000
    do
        echo "Matrix size: $matrix_size. T: $num_threads"

        # Run the program 5 times
        for run in {1..5}
        do
            echo "   Run $run"
            ./test $matrix_size $num_threads >> $output_file
        done
    done
done

echo "All executions completed. Results saved in $output_file"