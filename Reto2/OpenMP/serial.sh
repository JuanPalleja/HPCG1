#!/bin/bash

gcc -O3 -DUSE_CLOCK -O3 jacobi_openMP.c timing.c -o test -fopenmp

output_file="timing_serial.csv"

> $output_file

# Loop to run the program multiple times
for H in 8
do
    for N in 100000 200000 300000 400000 500000
    do
        for NSTEPS in 1000 2000 3000 4000 5000
        do
            for run in {1..5}
            do
                echo "   Run $run"
                ./test $H $N $NSTEPS u_serial.out >> $output_file
            done
        done
    done
done

echo "All executions completed. Results saved in $output_file"