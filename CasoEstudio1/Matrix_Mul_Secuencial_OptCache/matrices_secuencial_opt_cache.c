#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Función para generar números aleatorios entre 1 y 5
int generarNumeroAleatorio()
{
    return rand() % 5 + 1;
}

// Función para imprimir una matriz
void imprimirMatriz(int **matriz, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }
}

// Función para multiplicar dos matrices cuadradas
void multiplicarMatrices(int **matrizA, int **matrizB, int **resultado, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            resultado[i][j] = 0;
            for (int k = 0; k < n; k++)
            {
                resultado[i][j] += matrizA[i][k] * matrizB[j][k];
            }
        }
    }
}

int main(int argc, char *argv[])
{
    // Verificar si se proporcionó el valor de n como argumento
    if (argc != 2)
    {
        printf("Uso: %s <valor_de_n>\n", argv[0]);
        return 1;
    }

    // Obtener el valor de n desde el argumento de línea de comandos
    int n = atoi(argv[1]);

    // Inicializar la semilla para generar números aleatorios
    srand(time(NULL));

    // Crear matrices y reservar memoria dinámica
    int **matrizA, **matrizB, **resultado;
    matrizA = (int **)malloc(n * sizeof(int *));
    matrizB = (int **)malloc(n * sizeof(int *));
    resultado = (int **)malloc(n * sizeof(int *));

    for (int i = 0; i < n; i++)
    {
        matrizA[i] = (int *)malloc(n * sizeof(int));
        matrizB[i] = (int *)malloc(n * sizeof(int));
        resultado[i] = (int *)malloc(n * sizeof(int));
    }

    // Inicializar las matrices con valores aleatorios
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            matrizA[i][j] = generarNumeroAleatorio();
            matrizB[i][j] = generarNumeroAleatorio();
        }
    }

    // Imprimir las matrices originales
    // printf("Matriz A:\n");
    // imprimirMatriz(matrizA, n);

    // printf("\nMatriz B:\n");
    // imprimirMatriz(matrizB, n);

    // Medir el tiempo antes de ejecutar la función de multiplicación
    clock_t inicio = clock();

    // Multiplicar las matrices y almacenar el resultado en la matriz resultado
    multiplicarMatrices(matrizA, matrizB, resultado, n);

    // Medir el tiempo después de ejecutar la función de multiplicación
    clock_t fin = clock();

    // Calcular el tiempo transcurrido
    double tiempo_transcurrido = ((double)(fin - inicio)) / CLOCKS_PER_SEC;
    tiempo_transcurrido = tiempo_transcurrido * 1000; // convertir a milisegundos

    // Imprimir la matriz resultante
    // printf("\nMatriz Resultado:\n");
    // imprimirMatriz(resultado, n);

    // Imprimir el tiempo de ejecución
    // printf("\nTiempo de ejecucion: %.6f milisegundos para N=%i\n", tiempo_transcurrido, n);
    printf("%f,%d\n", tiempo_transcurrido, n);

    // Liberar la memoria
    for (int i = 0; i < n; i++)
    {
        free(matrizA[i]);
        free(matrizB[i]);
        free(resultado[i]);
    }
    free(matrizA);
    free(matrizB);
    free(resultado);

    return 0;
}
