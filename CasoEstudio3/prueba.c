// A demonstration of parallel computing in C using the MPI library. This
// program multiplies a set of square matrices using a manager-worker paradigm,
// where the workload is distributed among available worker processes and
// coordinated by a manager process.

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mpi.h"

// Size of the matrix (NxN)

MPI_Status status;

// Whether to print the matrix when completed
bool printResults = true;

// Print matrix function declaration

/**
 * @brief Prints the contents of an NxN matrix
 * 
 * @param matrix An NxN matrix of integers
 */
// Function to print a matrix
void printMatrix(int **matrix, int N) {
    int i, j;
    for (i = 0; i < N; ++i) {
        for (j = 0; j < N; ++j) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

// Counter variables
int i, j, k;

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s <matrix_size>\n", argv[0]);
        return 1;  // Exit with an error code
    }

    int N = atoi(argv[1]);

    int matrix1[N][N];
    int matrix2[N][N];
    int productMatrix[N][N];

    // Check for valid input
    if (N <= 0) {
        printf("Invalid input. N should be greater than 0.\n");
        return 1;  // Exit with an error code
    }

    int numberOfProcessors;
    int processorRank;
    int numberOfWorkers;

    // Processor sending data
    int sourceProcessor;

    // Processor to receive data
    int destinationProcessor;

    // The number of rows for a worker processor to process
    int rows;

    // The subset of a matrix to be processed by workers
    int matrixSubset;

    // Initialize MPI environment
    MPI_Init(&argc, &argv);

    // Determine number of processors available
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcessors);

    // Determine rank of calling process
    MPI_Comm_rank(MPI_COMM_WORLD, &processorRank);

    numberOfWorkers = numberOfProcessors - 1;

    double start_time, end_time;

    /* ---------- Manager Processor Code ---------- */

    if (processorRank == 0)
    {
        // Initialize a timer
        start_time = MPI_Wtime();

        // Populate the matrices with values
        srand(time(NULL));
        for (i = 0; i < N; i++)
        {
            for (j = 0; j < N; j++)
            {
                matrix1[i][j] = rand() % 9 + 1; 
                matrix2[i][j] = rand() % 9 + 1; 
            }
        }

        /* Send the matrix to the worker processes */
        rows = N / numberOfWorkers;
        matrixSubset = 0;

        // Iterate through all of the workers and assign work
        for (destinationProcessor = 1; destinationProcessor <= numberOfWorkers; destinationProcessor++)
        {
            // Determine the subset of the matrix to send to the destination processor
            MPI_Send(&matrixSubset, 1, MPI_INT, destinationProcessor, 1, MPI_COMM_WORLD);

            // Send the number of rows to process to the destination worker processor
            MPI_Send(&rows, 1, MPI_INT, destinationProcessor, 1, MPI_COMM_WORLD);

            // Send rows from matrix 1 to destination worker processor
            MPI_Send(&matrix1[matrixSubset][0], rows * N, MPI_INT, destinationProcessor, 1, MPI_COMM_WORLD);

            // Send entire matrix 2 to destination worker processor
            MPI_Send(&matrix2, N * N, MPI_INT, destinationProcessor, 1, MPI_COMM_WORLD);

            // Determine the next chunk of data to send to the next processor
            matrixSubset = matrixSubset + rows;
        }

        // Retrieve results from all workers processors
        for (i = 1; i <= numberOfWorkers; i++)
        {
            sourceProcessor = i;
            MPI_Recv(&matrixSubset, 1, MPI_INT, sourceProcessor, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, sourceProcessor, 2, MPI_COMM_WORLD, &status);
            MPI_Recv(&productMatrix[matrixSubset][0], rows * N, MPI_INT, sourceProcessor, 2, MPI_COMM_WORLD, &status);
        }

        // Stop the timer
        end_time = MPI_Wtime();

        // Optionally print  matrix results
        if (printResults == true)
        {
            int **ma = (int **)malloc(N * sizeof(int *));
            for (int i = 0; i < N; i++) {
                ma[i] = matrix1[i]; // Point to each row of the original matrix
            }
            int **mb = (int **)malloc(N * sizeof(int *));
            for (int i = 0; i < N; i++) {
                mb[i] = matrix2[i]; // Point to each row of the original matrix
            }
            int **rs = (int **)malloc(N * sizeof(int *));
            for (int i = 0; i < N; i++) {
                rs[i] = productMatrix[i]; // Point to each row of the original matrix
            }
            printf("Matrix 1:\n");
            printMatrix(ma,N);
            printf("Matrix 2:\n");
            printMatrix(mb,N);
            printf("Product Matrix:\n");
            printMatrix(rs,N);

            free(ma);
            free(mb);
            free(rs);
        }

        // Determine and print the total run time
        double runTime = end_time - start_time;
        printf("Runtime: %f seconds\n", runTime);
    }

    /* ---------- Worker Processor Code ---------- */

    if (processorRank > 0)
    {
        sourceProcessor = 0;
        MPI_Recv(&matrixSubset, 1, MPI_INT, sourceProcessor, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, sourceProcessor, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&matrix1, rows * N, MPI_INT, sourceProcessor, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&matrix2, N * N, MPI_INT, sourceProcessor, 1, MPI_COMM_WORLD, &status);

        /* Perform matrix multiplication */
        for (k = 0; k < N; k++)
        {
            for (i = 0; i < rows; i++)
            {
                productMatrix[i][k] = 0.0;
                for (j = 0; j < N; j++)
                {
                    productMatrix[i][k] = productMatrix[i][k] + matrix1[i][j] * matrix2[j][k];
                }
            }
        }

        MPI_Send(&matrixSubset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
        MPI_Send(&productMatrix, rows * N, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}

