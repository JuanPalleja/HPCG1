#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void multiplyMatrices(int **A, int **B, int **result, int N) {
    int i, j, k;

    for (i = 0; i < N; ++i) {
        for (j = 0; j < N; ++j) {
            result[i][j] = 0;
            for (k = 0; k < N; ++k) {
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
    clock_t begin = clock();

    // Multiply matrices A and B
    multiplyMatrices(matrixA, matrixB, result, N);

    clock_t end = clock();

    // Calculate the elapsed time
    double time_spent = ((double)(end - begin) / CLOCKS_PER_SEC)*1000;


    /*
    // Display the input matrices
    printf("Matrix A:\n");
    printMatrix(matrixA, N);

    printf("\nMatrix B:\n");
    printMatrix(matrixB, N);

    // Display the result
    printf("\nResultant matrix after multiplication:\n");
    printMatrix(result, N);
    **/

    // Display the elapsed time
    printf("Time: %f ms. N: %i\n", time_spent,N);

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
