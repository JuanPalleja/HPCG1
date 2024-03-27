#!/bin/bash

# Compile the C program
gcc timing.c jacobi1d.c -o test

# Specify the output file
output_file="resultado_jacobi1d_hilos.txt"

> $output_file

# Loop to run the program multiple times 
for h in 1 2 4 8 12 1000
do
    for n in 100000
    do
        for steps in 1000
        do
            for run in {1..1}
            do
                echo "   Run $run"
                ./test $n $steps $h >> $output_file
            done
        done
    done
done

echo "All executions completed. Results saved in $output_file"