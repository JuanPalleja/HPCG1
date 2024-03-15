#!/bin/bash
#SBATCH --partition=all  	#Seleccione los nodos para el trabajo de todos el conjunto de nodos de cómputo del cluster
#SBATCH -o Serial.%j.out    	#Nombre del archivo de salida
#SBATCH -J Serial	        #Nombre del trabajo

source /usr/local/intel/parallel_studio_xe_2016.3.067/psxevars.sh intel64 2> /dev/null
ulimit -s unlimited -c unlimited

N=100000
NSTEPS=1000
./jacobi1d $N $NSTEPS u_serial.out > timing_serial.out
