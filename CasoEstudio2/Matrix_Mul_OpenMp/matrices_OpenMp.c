#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <sys/time.h> 


void multiplyMatrices(int **A, int **B, int **result, int N) {
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            result[i][j] = 0;
            for (int k = 0; k < N; ++k) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

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

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <matrix_size> <num_threads>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int numThreads = atoi(argv[2]);

    if (N <= 0 || numThreads <= 0) {
        printf("Invalid input. N and num_threads should be greater than 0.\n");
        return 1;
    }

    omp_set_num_threads(numThreads);

    // Check for valid input
    if (N <= 0) {
        printf("Invalid input. N should be greater than 0.\n");
        return 1;  // Exit with an error code
    }

    // Seed the random number generator
    srand(time(NULL));

    // Dynamically allocate memory for matrices
    int **matrixA, **matrixB, **result;
    matrixA = (int **)malloc(N * sizeof(int *));
    matrixB = (int **)malloc(N * sizeof(int *));
    result = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; ++i) {
        matrixA[i] = (int *)malloc(N * sizeof(int));
        matrixB[i] = (int *)malloc(N * sizeof(int));
        result[i] = (int *)malloc(N * sizeof(int));
    }

    // Fill matrices A and B with random numbers between 1 and 9
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrixA[i][j] = rand() % 9 + 1;  // Random number between 1 and 9
            matrixB[i][j] = rand() % 9 + 1;  // Random number between 1 and 9
        }
    }

    // Measure the start time
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Multiply matrices A and B
    multiplyMatrices(matrixA, matrixB, result, N);

    gettimeofday(&end, NULL);

    // Calcula el tiempo transcurrido
    double time_spent = ((end.tv_sec - start.tv_sec) * 1000.0 +
                         (end.tv_usec - start.tv_usec) / 1000.0);

    // Display the elapsed time
    printf("%d,%f,%i\n",numThreads, time_spent, N);


    //printMatrix(matrixA,N);
    //printMatrix(matrixB,N);
    //printMatrix(result,N);

    // Free dynamically allocated memory
    for (int i = 0; i < N; ++i) {
        free(matrixA[i]);
        free(matrixB[i]);
        free(result[i]);
    }
    free(matrixA);
    free(matrixB);
    free(result);

    return 0;
}
