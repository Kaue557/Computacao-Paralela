#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int N, num_threads;
double *A, *B, *C;

void* matmul_thread(void* arg) {
    long my_rank = (long)arg;
    
    // Partição contígua das linhas para evitar false sharing
    int linhas_por_thread = N / num_threads;
    int inicio = my_rank * linhas_por_thread;
    int fim = (my_rank == num_threads - 1) ? N : inicio + linhas_por_thread;

    for (int i = inicio; i < fim; i++) {
        for (int j = 0; j < N; j++) {
            double soma = 0.0;
            for (int k = 0; k < N; k++) {
                soma += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = soma;
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <N> <num_threads>\n", argv[0]);
        return 1;
    }
    
    N = atoi(argv[1]);
    num_threads = atoi(argv[2]);

    A = (double*) malloc(N * N * sizeof(double));
    B = (double*) malloc(N * N * sizeof(double));
    C = (double*) malloc(N * N * sizeof(double));

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i * N + j] = (double)(i + j);
            B[i * N + j] = (double)(i * j);
        }
    }

    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    struct timespec inicio_tempo, fim_tempo;

    clock_gettime(CLOCK_MONOTONIC, &inicio_tempo);

    for (long i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, matmul_thread, (void*)i);
    }
    for (long i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &fim_tempo);
    double tempo = (fim_tempo.tv_sec - inicio_tempo.tv_sec) + (fim_tempo.tv_nsec - inicio_tempo.tv_nsec) / 1e9;

    printf("[Pthreads Padrão] N: %d | Threads: %d | Amostra C[N-1]: %.2f | Tempo: %.4f s\n", N, num_threads, C[N * N - 1], tempo);

    free(A);
    free(B);
    free(C);
    free(threads);

    return 0;
}