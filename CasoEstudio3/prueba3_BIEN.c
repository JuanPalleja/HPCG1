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

    double start_time, end_time;

    int *matrixA = NULL;
    int *matrixB = NULL;
    int *result = NULL;

    if (rank == 0) {
        matrixA = (int *)malloc(N * N * sizeof(int));
        matrixB = (int *)malloc(N * N * sizeof(int));
        result = (int *)malloc(N * N * sizeof(int));

        srand(time(NULL));
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                matrixA[i * N + j] = rand() % 9 + 1;
                matrixB[i * N + j] = rand() % 9 + 1;
            }
        }

        start_time = MPI_Wtime(); // Start timing after initialization
    } else {
        // Allocate space for matrixB in all processes
        matrixB = (int *)malloc(N * N * sizeof(int));
    }

    int rows_per_process = N / num_procs;
    int extra_rows = N % num_procs;

    int start_row = rank * rows_per_process + (rank < extra_rows ? rank : extra_rows);
    int end_row = start_row + rows_per_process + (rank < extra_rows ? 1 : 0);

    int *local_A = (int *)malloc((end_row - start_row) * N * sizeof(int));
    int *local_result = (int *)malloc((end_row - start_row) * N * sizeof(int));

    if (rank == 0) {
        for (int i = 1; i < num_procs; ++i) {
            int other_start_row = i * rows_per_process + (i < extra_rows ? i : extra_rows);
            int other_end_row = other_start_row + rows_per_process + (i < extra_rows ? 1 : 0);
            MPI_Send(matrixA + other_start_row * N, (other_end_row - other_start_row) * N, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        // Copy the local part of matrixA to local_A for rank 0
        for (int i = start_row; i < end_row; ++i) {
            for (int j = 0; j < N; ++j) {
                local_A[(i - start_row) * N + j] = matrixA[i * N + j];
            }
        }
    } else {
        MPI_Recv(local_A, (end_row - start_row) * N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Broadcast matrixB to all processes
    MPI_Bcast(matrixB, N * N, MPI_INT, 0, MPI_COMM_WORLD);

    multiplyMatrices(local_A, matrixB, local_result, 0, end_row - start_row, N);

    MPI_Gather(local_result, (end_row - start_row) * N, MPI_INT, result, (end_row - start_row) * N, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        end_time = MPI_Wtime(); // End timing after gathering results

        printf("%i,%f\n",N, end_time - start_time);

        free(matrixA);
        free(matrixB);
        free(result);
    } else {
        free(matrixB);
    }

    free(local_A);
    free(local_result);

    MPI_Finalize();
    return 0;
}
