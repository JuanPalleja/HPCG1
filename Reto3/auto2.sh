#!/bin/bash

mpicc automaton_mpi.c -o auto2_exe

scp /home/cluser/auto2_exe cluser@wn1:/home/cluser/auto2_exe
scp /home/cluser/auto2_exe cluser@wn2:/home/cluser/auto2_exe
scp /home/cluser/auto2_exe cluser@wn3:/home/cluser/auto2_exe


# Specify the output file
output_file="resultado_automaton_mpi.csv"

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
            mpiexec -hosts head,wn1,wn2,wn3 ./auto2_exe $n $s >> $output_file
        done
    done
done

echo "All executions completed. Results saved in $output_file"
