#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

long long N;
int num_threads;
float *A, *B, *C;

void* somar_vetores(void* arg) {
    long my_rank = (long)arg;
    
    // Determina o tamanho do bloco contíguo para cada thread
    long long local_n = N / num_threads;
    long long inicio = my_rank * local_n;
    // Se tiver, a última thread processa o resto da divisão
    long long fim = (my_rank == num_threads - 1) ? N : inicio + local_n;

    // Soma contígua sem necessidade de mutex (posições independentes)
    for (long long i = inicio; i < fim; i++) {
        C[i] = A[i] + B[i];
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Uso: %s <N> <num_threads>\n", argv[0]);
        return 1;
    }

    N = atoll(argv[1]);
    num_threads = atoi(argv[2]);

    // Alocação dinâmica dos vetores gigantes
    A = (float*) malloc(N * sizeof(float));
    B = (float*) malloc(N * sizeof(float));
    C = (float*) malloc(N * sizeof(float));

    if (A == NULL || B == NULL || C == NULL) {
        printf("Erro na alocação de memória.\n");
        return 1;
    }

    // Preenchimento inicial (executado sequencialmente pela thread principal)
    for (long long i = 0; i < N; i++) {
        A[i] = (float)i * 0.5f;
        B[i] = (float)i * 1.5f;
    }

    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    struct timespec inicio_tempo, fim_tempo;

    clock_gettime(CLOCK_MONOTONIC, &inicio_tempo);

    for (long i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, somar_vetores, (void*)i);
    }

    for (long i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &fim_tempo);

    double tempo_gasto = (fim_tempo.tv_sec - inicio_tempo.tv_sec) +
                         (fim_tempo.tv_nsec - inicio_tempo.tv_nsec) / 1e9;

    printf("Soma concluída. Amostra C[N-1]: %.2f\n", C[N-1]);
    printf("Tempo: %.6f segundos\n", tempo_gasto);

    free(A);
    free(B);
    free(C);
    free(threads);

    return 0;
}