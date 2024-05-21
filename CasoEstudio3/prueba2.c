#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// Function to print a matrix
void printMatrix(int *matrix, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            printf("%d ", matrix[i * cols + j]);
        }
        printf("\n");
    }
}

// Function to multiply matrices
void multiplyMatrices(int *A, int *B, int *result, int start_row, int end_row, int N) {
    for (int i = start_row; i < end_row; ++i) {
        for (int j = 0; j < N; ++j) {
            result[i * N + j] = 0;
            for (int k = 0; k < N; ++k) {
                result[i * N + j] += A[i * N + k] * B[k * N + j];
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <matrix_size>\n", argv[0]);
        return 1;  // Exit with an error code
    }

    int N = atoi(argv[1]);
    if (N <= 0) {
        printf("Invalid input. N should be greater than 0.\n");
        return 1;  // Exit with an error code
    }

    int rank, num_procs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if (num_procs != 4) {
        if (rank == 0) {
            printf("This program must be run with 4 processes.\n");
        }
        MPI_Finalize();
        return 1;
    }

    int rows_per_process = N / (num_procs - 1);
    double start_time, end_time;

    if (rank == 0) {
        srand(time(NULL));
        int *matrixA = (int *)malloc(N * N * sizeof(int));
        int *matrixB = (int *)malloc(N * N * sizeof(int));
        int *result = (int *)malloc(N * N * sizeof(int));

        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                matrixA[i * N + j] = rand() % 9 + 1;
                matrixB[i * N + j] = rand() % 9 + 1;
            }
        }

        start_time = MPI_Wtime();

        for (int dest = 1; dest < num_procs; dest++) {
            MPI_Send(matrixB, N * N, MPI_INT, dest, 0, MPI_COMM_WORLD);
            MPI_Send(&matrixA[(dest - 1) * rows_per_process * N], rows_per_process * N, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }

        for (int src = 1; src < num_procs; src++) {
            MPI_Recv(&result[(src - 1) * rows_per_process * N], rows_per_process * N, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        end_time = MPI_Wtime();

        printf("Matrix A:\n");
        printMatrix(matrixA, N, N);
        printf("Matrix B:\n");
        printMatrix(matrixB, N, N);
        printf("Result Matrix:\n");
        printMatrix(result, N, N);
        printf("Time taken: %f seconds\n", end_time - start_time);

        free(matrixA);
        free(matrixB);
        free(result);
    } else {
        int *matrixA = (int *)malloc(rows_per_process * N * sizeof(int));
        int *matrixB = (int *)malloc(N * N * sizeof(int));
        int *result = (int *)malloc(rows_per_process * N * sizeof(int));

        MPI_Recv(matrixB, N * N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(matrixA, rows_per_process * N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        multiplyMatrices(matrixA, matrixB, result, 0, rows_per_process, N);

        MPI_Send(result, rows_per_process * N, MPI_INT, 0, 0, MPI_COMM_WORLD);

        free(matrixA);
        free(matrixB);
        free(result);
    }

    MPI_Finalize();
    return 0;
}
