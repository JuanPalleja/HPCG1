#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void llenar_lista(int n, int lista[]) {
    for (int i = 0; i < n; i++) {
        lista[i] = rand() % 2;  // Genera 0 o 1
    }
}

void realizar_barridos(int n, int lista[], int s) {
    for (int barrido = 0; barrido < s; barrido++) {
        for (int i = 0; i < n; i++) {
            if (lista[i] == 1) {
                int siguiente = (i + 1) % n;  // Siguiente posición, con wrap-around
                if (lista[siguiente] == 0) {
                    lista[siguiente] = 1;
                    lista[i] = 0;
                    i++;
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <valor de N> <valor de S>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    int s = atoi(argv[2]);

    if (n <= 0 || s < 0) {
        printf("El valor de N debe ser un entero positivo y el valor de S no debe ser negativo.\n");
        return 1;
    }

    // Semilla para la generación de números aleatorios
    srand(time(NULL));

    // Crear una lista para almacenar los números aleatorios
    int lista[n];

    // Llenar la lista con números aleatorios entre 0 y 1
    llenar_lista(n, lista);

    clock_t start = clock();
    realizar_barridos(n, lista, s);
    clock_t end = clock();

    double time_spent = ((double)(end - start) / CLOCKS_PER_SEC);

    printf("%i,%i,%f\n",n,s, time_spent);

    return 0;
}
