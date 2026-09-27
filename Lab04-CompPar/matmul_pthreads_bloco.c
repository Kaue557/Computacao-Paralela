#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int N, num_threads, bloco_B;
double *A, *B, *C;

void* matmul_thread_bloco(void* arg) {
    long my_rank = (long)arg;
    
    // Divisão das linhas entre as threads
    int linhas_por_thread = N / num_threads;
    int inicio = my_rank * linhas_por_thread;
    int fim = (my_rank == num_threads - 1) ? N : inicio + linhas_por_thread;

    // Tiling restrito apenas à faixa de linhas desta thread
    for (int ii = inicio; ii < fim; ii += bloco_B) {
        for (int jj = 0; jj < N; jj += bloco_B) {
            for (int kk = 0; kk < N; kk += bloco_B) {
                
                // Multiplicação do bloco otimizado
                for (int i = ii; i < ii + bloco_B && i < fim; i++) {
                    for (int k = kk; k < kk + bloco_B && k < N; k++) {
                        double r = A[i * N + k];
                        for (int j = jj; j < jj + bloco_B && j < N; j++) {
                            C[i * N + j] += r * B[k * N + j];
                        }
                    }
                }
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Uso: %s <N> <num_threads> <Bloco B>\n", argv[0]);
        return 1;
    }
    
    N = atoi(argv[1]);
    num_threads = atoi(argv[2]);
    bloco_B = atoi(argv[3]);

    A = (double*) malloc(N * N * sizeof(double));
    B = (double*) malloc(N * N * sizeof(double));
    C = (double*) calloc(N * N, sizeof(double)); // calloc para acumulo (+=)

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
        pthread_create(&threads[i], NULL, matmul_thread_bloco, (void*)i);
    }
    for (long i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &fim_tempo);
    double tempo = (fim_tempo.tv_sec - inicio_tempo.tv_sec) + (fim_tempo.tv_nsec - inicio_tempo.tv_nsec) / 1e9;

    printf("[Pthreads Blocado] N: %d | Threads: %d | Bloco B: %d | Amostra: %.2f | Tempo: %.4f s\n", N, num_threads, bloco_B, C[N * N - 1], tempo);

    free(A);
    free(B);
    free(C);
    free(threads);

    return 0;
}