#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define TAG_INITIAL_LIST 1
#define TAG_RESULT_LIST 2

void llenar_lista(int n, int lista[]) {
    for (int i = 0; i < n; i++) {
        lista[i] = rand() % 2;  // Genera 0 o 1
    }
}

void realizar_barridos(int n, int lista[], int s, int inicio, int fin, MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    // Crear buffer para almacenar el extremo derecho de la sublista
    int derecha;

    for (int barrido = 0; barrido < s; barrido++) {
        // Intercambiar extremos entre procesos vecinos
        if (rank > 0) {
            MPI_Send(&lista[inicio], 1, MPI_INT, rank - 1, TAG_INITIAL_LIST, comm);
            MPI_Recv(&lista[inicio - 1], 1, MPI_INT, rank - 1, TAG_INITIAL_LIST, comm, MPI_STATUS_IGNORE);
        }
        if (rank < size - 1) {
            MPI_Send(&lista[fin - 1], 1, MPI_INT, rank + 1, TAG_INITIAL_LIST, comm);
            MPI_Recv(&derecha, 1, MPI_INT, rank + 1, TAG_INITIAL_LIST, comm, MPI_STATUS_IGNORE);
        }

        // Realizar los barridos en la sublista local
        for (int i = inicio; i < fin; i++) {
            if (lista[i] == 1) {
                int siguiente = (i + 1) % n;  // Siguiente posición, con wrap-around
                if (lista[siguiente] == 0) {
                    lista[siguiente] = 1;
                    lista[i] = 0;
                    // Avanzamos al siguiente
                    i++;
                }
            }
        }

        // Copiar el extremo derecho de la sublista vecina
        if (rank < size - 1) {
            lista[fin] = derecha;
        }
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3) {
        if (rank == 0) {
            printf("Uso: %s <valor de N> <valor de S>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int n = atoi(argv[1]);
    int s = atoi(argv[2]);

    if (n <= 0 || s < 0) {
        if (rank == 0) {
            printf("El valor de N debe ser un entero positivo y el valor de S no debe ser negativo.\n");
        }
        MPI_Finalize();
        return 1;
    }

    srand(time(NULL) + rank); // Semilla para la generación de números aleatorios

    int chunk_size = (n + size - 1) / size;  // Tamaño del trozo (chunk) para cada proceso
    int inicio = rank * chunk_size;
    int fin = (rank + 1) * chunk_size;
    if (fin > n) {
        fin = n;
    }

    double start_time, end_time;
    start_time = MPI_Wtime();

    // Declarar las sublistas de tamaño fijo.
    int sublista[chunk_size];
    for (int i = inicio; i < fin; i++) {
        sublista[i - inicio] = rand() % 2;
    }

    // El nodo maestro recopila la lista inicial completa
    int lista_completa[n];
    if (rank == 0) {
        llenar_lista(n, lista_completa);
    }

    // Compartir la sublista inicial con cada proceso
    MPI_Scatter(lista_completa, chunk_size, MPI_INT, sublista, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Cada proceso MPI realiza los barridos en su parte de la lista
    realizar_barridos(n, sublista, s, 0, fin - inicio, MPI_COMM_WORLD);

    // El nodo maestro recibe las partes de la lista resultante de cada proceso MPI
    MPI_Gather(sublista, chunk_size, MPI_INT, lista_completa, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Imprimir la lista resultante completa en el nodo maestro
    if (rank == 0) {
        end_time = MPI_Wtime();
        printf("%i,%i,%f\n",n,s, end_time - start_time);
        // Para verificar, se puede imprimir la lista completa:
        // for (int i = 0; i < n; i++) {
        //     printf("%d ", lista_completa[i]);
        // }
        // printf("\n");
    }

    MPI_Finalize();
    return 0;
}
