#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h> 

// Estructura para pasar múltiples argumentos a la función de hilo
struct ThreadArgs {
    int **A;
    int **B;
    int **result;
    int startRow;
    int endRow;
    int N;
};

// Función de multiplicación de matrices para cada hilo
void *multiplyMatrices(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;

    for (int i = threadArgs->startRow; i < threadArgs->endRow; ++i) {
        for (int j = 0; j < threadArgs->N; ++j) {
            threadArgs->result[i][j] = 0;
            for (int k = 0; k < threadArgs->N; ++k) {
                threadArgs->result[i][j] += threadArgs->A[i][k] * threadArgs->B[k][j];
            }
        }
    }

    return NULL;
}

// Función para imprimir una matriz
void printMatrix(int **matrix, int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
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

    srand(time(NULL));

    // Asignación dinámica de memoria para matrices
    int **matrixA, **matrixB, **result;
    matrixA = (int **)malloc(N * sizeof(int *));
    matrixB = (int **)malloc(N * sizeof(int *));
    result = (int **)malloc(N * sizeof(int *));
    for (int i = 0; i < N; ++i) {
        matrixA[i] = (int *)malloc(N * sizeof(int));
        matrixB[i] = (int *)malloc(N * sizeof(int));
        result[i] = (int *)malloc(N * sizeof(int));
    }

    // Inicialización de matrices A y B con números aleatorios entre 1 y 9
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrixA[i][j] = rand() % 9 + 1;
            matrixB[i][j] = rand() % 9 + 1;
        }
    }

    // Medición del tiempo de inicio
    struct timeval start, end;
    gettimeofday(&start, NULL);

    // Creación de hilos
    pthread_t threads[numThreads];
    struct ThreadArgs threadArgs[numThreads];

    for (int i = 0; i < numThreads; ++i) {
        threadArgs[i].A = matrixA;
        threadArgs[i].B = matrixB;
        threadArgs[i].result = result;
        threadArgs[i].startRow = i * (N / numThreads);
        threadArgs[i].endRow = (i == numThreads - 1) ? N : (i + 1) * (N / numThreads);
        threadArgs[i].N = N;

        pthread_create(&threads[i], NULL, multiplyMatrices, (void *)&threadArgs[i]);
    }

    // Espera a que todos los hilos terminen
    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&end, NULL);

    // Calcula el tiempo transcurrido
    double time_spent = ((end.tv_sec - start.tv_sec) * 1000.0 +
                         (end.tv_usec - start.tv_usec) / 1000.0);

    // Muestra el tiempo transcurrido
    printf("%f;%d;%d\n", time_spent, N, numThreads);

    /*
    // Muestra las matrices de entrada y el resultado
    printf("Matrix A:\n");
    printMatrix(matrixA, N);

    printf("\nMatrix B:\n");
    printMatrix(matrixB, N);

    printf("\nResultant matrix after multiplication:\n");
    printMatrix(result, N);
    */

    // Libera la memoria asignada dinámicamente
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
