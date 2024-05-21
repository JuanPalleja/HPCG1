#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

void multiplyMatrices(int **A, int **B, int **result, int start_row, int end_row, int N) {
    int i, j, k;

    for (i = start_row; i < end_row; ++i) {
        for (j = 0; j < N; ++j) {
            result[i][j] = 0;
            for (k = 0; k < N; ++k) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void printMatrix(int **matrix, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <matrix_size>\n", argv[0]);
        return 1;  // Exit with an error code
    }

    int N = atoi(argv[1]);

    // Check for valid input
    if (N <= 0) {
        printf("Invalid input. N should be greater than 0.\n");
        return 1;  // Exit with an error code
    }

    int rank, num_procs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (num_procs != 4) {
        printf("This program must be run with 4 processes.\n");
        MPI_Finalize();
        return 1;
    }

    // Seed the random number generator only on the head node
    if (rank == 0) {
        srand(time(NULL));
    }

    double start_time, end_time;

    if (rank == 0) {
        // Dynamically allocate memory for matrices in the head node
        int **matrixA, **matrixB, **result;
        matrixA = (int **)malloc(N * sizeof(int *));
        matrixB = (int **)malloc(N * sizeof(int *));
        result = (int **)malloc(N * sizeof(int *));
        for (int i = 0; i < N; ++i) {
            matrixA[i] = (int *)malloc(N * sizeof(int));
            matrixB[i] = (int *)malloc(N * sizeof(int));
            result[i] = (int *)malloc(N * sizeof(int));
        }

        // Fill matrix B with random numbers between 1 and 9
        if (rank == 0) {
            for (int i = 0; i < N; ++i) {
                for (int j = 0; j < N; ++j) {
                    matrixB[i][j] = rand() % 9 + 1;  // Random number between 1 and 9
                }
            }
        }

        // Generate matrix A on the head node
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                matrixA[i][j] = rand() % 9 + 1;  // Random number between 1 and 9
            }
        }

        start_time = MPI_Wtime();

        // Send matrix B to all worker nodes
        for (int dest = 1; dest < num_procs; dest++) {
            MPI_Send(&(matrixB[0][0]), N*N, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }

        // Send portions of matrix A to worker nodes
        int rows_per_process = N / (num_procs - 1);
        for (int dest = 1; dest < num_procs; dest++) {
            MPI_Send(&(matrixA[(dest - 1) * rows_per_process][0]), rows_per_process * N, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }

        // Receive partial results from each worker node
        for (int src = 1; src < num_procs; src++) {
            MPI_Recv(&(result[(src - 1) * rows_per_process][0]), rows_per_process * N, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        end_time = MPI_Wtime();

        // Print matrices and result matrix
        printf("Matrix A:\n");
        printMatrix(matrixA, N, N);
        printf("Matrix B:\n");
        printMatrix(matrixB, N, N);
        printf("Result Matrix:\n");
        printMatrix(result, N, N);

        printf("Time taken: %f seconds\n", end_time - start_time);

        // Free dynamically allocated memory in the head node
        for (int i = 0; i < N; ++i) {
            free(matrixA[i]);
            free(matrixB[i]);
            free(result[i]);
        }
        free(matrixA);
        free(matrixB);
        free(result);
    } else {
        // Dynamically allocate memory for matrices A, B, and result in worker nodes
        int **matrixA, **matrixB, **result;
        matrixA = (int **)malloc((N / (num_procs - 1)) * sizeof(int *));
        matrixB = (int **)malloc(N * sizeof(int *));
        result = (int **)malloc((N / (num_procs - 1)) * sizeof(int *));
        for (int i = 0; i < (N / (num_procs - 1)); ++i) {
            matrixA[i] = (int *)malloc(N * sizeof(int));
            result[i] = (int *)malloc(N * sizeof(int));
        }
        for (int i = 0; i < N; ++i) {
            matrixB[i] = (int *)malloc(N * sizeof(int));
        }

        // Receive matrix B from the head node
        MPI_Recv(&(matrixB[0][0]), N*N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Receive portion of matrix A from the head node
        int rows_per_process = N / (num_procs - 1);
        MPI_Recv(&(matrixA[0][0]), rows_per_process * N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Start computation
        multiplyMatrices(matrixA, matrixB, result, 0, (N / (num_procs - 1)), N);

        // Send completion signal back to the head node
        int signal = 1;
        MPI_Send(&signal, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        // Free dynamically allocated memory in worker nodes
        for (int i = 0; i < (N / (num_procs - 1)); ++i) {
            free(matrixA[i]);
            free(result[i]);
        }
        free(matrixA);
        free(result);
        for (int i = 0; i < N; ++i) {
            free(matrixB[i]);
        }
        free(matrixB);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    return 0;
}
