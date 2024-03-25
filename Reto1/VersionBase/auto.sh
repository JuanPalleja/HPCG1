#!/bin/bash

# Compile the C program
gcc timing.c jacobi1d.c -o test

# Specify the output file
output_file="resultado_jacobi1d.txt"

> $output_file

# Loop to run the program multiple times 
for n in 1000
do
    for steps in 1000
    do
        for run in {1..2}
        do
            echo "   Run $run"
            ./test $n $steps >> $output_file
        done
    done
done

echo "All executions completed. Results saved in $output_file"