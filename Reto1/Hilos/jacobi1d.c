#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "timing.h"

typedef struct {
    int start;
    int end;
    int n;
    double h2;
    double *u;
    double *f;
} ThreadArgs;

void* jacobi_thread(void* arg) {
    ThreadArgs *args = (ThreadArgs*)arg;
    int i;
    double h2 = args->h2;
    double* u = args->u;
    double* f = args->f;
    int n = args->n;

    for (i = args->start; i < args->end; ++i) {
        u[i] = (u[i-1] + u[i+1] + h2*f[i])/2;
    }

    return NULL;
}

void jacobi(int nsweeps, int n, double* u, double* f,int NUM_THREADS) {
    int i, sweep;
    double h  = 1.0 / n;
    double h2 = h*h;
    pthread_t threads[NUM_THREADS];
    ThreadArgs args[NUM_THREADS];

    for (sweep = 0; sweep < nsweeps; sweep += 2) {
        int chunk_size = n / NUM_THREADS;
        
        /* Create and join threads */
        for (i = 0; i < NUM_THREADS; ++i) {
            args[i].start = i * chunk_size + 1;
            args[i].end = (i == NUM_THREADS - 1) ? n : (i + 1) * chunk_size;
            args[i].n = n;
            args[i].h2 = h2;
            args[i].u = u;
            args[i].f = f;
            pthread_create(&threads[i], NULL, jacobi_thread, &args[i]);
        }

        for (i = 0; i < NUM_THREADS; ++i) {
            pthread_join(threads[i], NULL);
        }
    }
}

/* The rest of the code remains unchanged */

void write_solution(int n, double* u, const char* fname)
{
    int i;
    double h = 1.0 / n;
    FILE* fp = fopen(fname, "w+");
    for (i = 0; i <= n; ++i)
        fprintf(fp, "%g %g\n", i*h, u[i]);
    fclose(fp);
}


int main(int argc, char** argv)
{
    int i;
    int n, nsteps, thread;
    double* u;
    double* f;
    double h;
    timing_t tstart, tend;
    char* fname;

    /* Process arguments */
    n      = (argc > 1) ? atoi(argv[1]) : 100;
    nsteps = (argc > 2) ? atoi(argv[2]) : 100;
    thread = (argc > 3) ? atoi(argv[3]) : 2;
    fname  = (argc > 4) ? argv[4] : NULL;
    h      = 1.0/n;

    /* Allocate and initialize arrays */
    u = (double*) malloc( (n+1) * sizeof(double) );
    f = (double*) malloc( (n+1) * sizeof(double) );
    memset(u, 0, (n+1) * sizeof(double));
    for (i = 0; i <= n; ++i)
        f[i] = i * h;

    /* Run the solver */
    get_time(&tstart);
    jacobi(nsteps, n, u, f, thread);
    get_time(&tend);

    /* Run the solver */    
    printf("n: %d\n"
           "nsteps: %d\n"
           "Elapsed time: %g s\n", 
           n, nsteps, timespec_diff(tstart, tend));

    /* Write the results */
    if (fname)
        write_solution(n, u, fname);

    free(f);
    free(u);
    return 0;
}
