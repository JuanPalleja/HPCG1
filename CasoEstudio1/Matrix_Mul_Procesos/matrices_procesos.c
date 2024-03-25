#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>

void multiplyMatrices(int **A, int **B, int **result, int startRow, int endRow, int N) {
    for (int i = startRow; i < endRow; ++i) {
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
        // printf("Usage: %s <matrix_size> <num_processes>\n", argv[0]);
        return 1;  // Exit with an error code
    }

    int N = atoi(argv[1]);
    int numProcesses = atoi(argv[2]);

    // Check for valid input
    if (N <= 0 || numProcesses <= 0) {
        // printf("Invalid input. N and num_processes should be greater than 0.\n");
        return 1;  // Exit with an error code
    }

    // Seed the random number generator
    srand(time(NULL));

    // Dynamically allocate memory for matrices
    int **matrixA, **matrixB;
    matrixA = (int **)malloc(N * sizeof(int *));
    matrixB = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; ++i) {
        matrixA[i] = (int *)malloc(N * sizeof(int));
        matrixB[i] = (int *)malloc(N * sizeof(int));
    }

    // Fill matrices A and B with random numbers between 1 and 9
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrixA[i][j] = rand() % 9 + 1;  // Random number between 1 and 9
            matrixB[i][j] = rand() % 9 + 1;  // Random number between 1 and 9
        }
    }

    // Create shared memory for the result matrix
    int **result = mmap(NULL, N * sizeof(int *), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    for (int i = 0; i < N; ++i) {
        result[i] = mmap(NULL, N * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    }

    //Result times
    int *resultTimes = mmap(NULL, numProcesses * sizeof(double *), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);


    // Measure the start time
    clock_t begin = clock();

    // Create processes
    for (int i = 0; i < numProcesses; ++i) {
        pid_t pid = fork();

        if (pid == 0) {  // Child process
            int startRow = i * (N / numProcesses);
            int endRow = (i == numProcesses - 1) ? N : (i + 1) * (N / numProcesses);

            clock_t beginForkChildTime = clock();
            multiplyMatrices(matrixA, matrixB, result, startRow, endRow, N);
            clock_t endForkChildTime = clock();
            double time_spent_fork_child = ((double)(endForkChildTime - beginForkChildTime) / CLOCKS_PER_SEC)*1000;
            resultTimes[i] = time_spent_fork_child;

            exit(0);  // Exit child process
        } else if (pid < 0) {  // Error handling
            // perror("Error in fork");
            return 1;
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < numProcesses; ++i) {
        wait(NULL);
    }

    clock_t end = clock();

    // Calculate the elapsed time
    double time_spent = ((double)(end - begin) / CLOCKS_PER_SEC)*1000;

    // Display the elapsed time
    // printf("Time: %f ms. N: %i. P: %i\n", time_spent,N,numProcesses);

    // printf("Hijooooos  -------------------------- \n");

    double max_value = 0.0;

    for(int i = 0; i < numProcesses; ++i) {
        double time_spent_child = resultTimes[i];
        // printf("Time: %f ms. Child %i.\n", time_spent_child,i);

        if(time_spent_child > max_value)
            max_value = time_spent_child;
    }

    // printf("max Time: %f ms. \n", max_value );
    double total_time = max_value + time_spent;
    // printf("total time: %f ms. \n", total_time);

    // printf("tiempoTotal;N;P");

    printf("%f;%d;%d\n", total_time, N, numProcesses);

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

    // Free dynamically allocated memory
    for (int i = 0; i < N; ++i) {
        free(matrixA[i]);
        free(matrixB[i]);
        munmap(result[i], N * sizeof(int));
    }
    free(matrixA);
    free(matrixB);
    munmap(result, N * sizeof(int *));

    return 0;
}
