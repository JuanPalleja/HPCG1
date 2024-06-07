#!/bin/bash

# Compile the C program
gcc automaton.c -o auto1_exe

# Specify the output file
output_file="resultado_automaton.csv"

> $output_file

# Loop to run the program multiple times for each matrix size
for s in 10000 20000 30000 40000 50000
do
    for n in 100000 200000 300000 400000 500000
    do
        echo "Vector size: $n. Sweeps: $s"

        # Run the program 5 times
        for run in {1..5}
        do
            echo "Run $run"
            ./auto1_exe $n $s >> $output_file
        done
    done
done

echo "All executions completed. Results saved in $output_file"