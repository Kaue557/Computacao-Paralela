#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

long long N;
int num_threads;
double *M, *V, *R;

void* multiplicar_matriz_vetor(void* arg) {
    long my_rank = (long)arg;
    
    // Divisão por blocos de linhas entre as threads
    long long local_n = N / num_threads;
    long long inicio = my_rank * local_n;
    long long fim = (my_rank == num_threads - 1) ? N : inicio + local_n;

    for (long long i = inicio; i < fim; i++) {
        // Variável local previne (false sharing) na memória cache
        double soma = 0.0;
        for (long long j = 0; j < N; j++) {
            soma += M[i * N + j] * V[j];
        }
        // Única escrita no vetor compartilhado
        R[i] = soma;
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

    // Alocação dinâmica. A matriz M é alocada como um vetor 1D contíguo (N x N)
    M = (double*) malloc(N * N * sizeof(double));
    V = (double*) malloc(N * sizeof(double));
    R = (double*) malloc(N * sizeof(double));

    if (M == NULL || V == NULL || R == NULL) {
        printf("Erro na alocação de memória. Matriz muito grande.\n");
        return 1;
    }

    // Inicialização da matriz M e vetor V sequencialmente
    for (long long i = 0; i < N; i++) {
        V[i] = 1.0; // Simplificado para validação
        for (long long j = 0; j < N; j++) {
            M[i * N + j] = (double)(i + j);
        }
    }

    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    struct timespec inicio_tempo, fim_tempo;

    clock_gettime(CLOCK_MONOTONIC, &inicio_tempo);

    for (long i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, multiplicar_matriz_vetor, (void*)i);
    }

    for (long i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &fim_tempo);

    double tempo_gasto = (fim_tempo.tv_sec - inicio_tempo.tv_sec) +
                         (fim_tempo.tv_nsec - inicio_tempo.tv_nsec) / 1e9;

    printf("Multiplicação concluída. Amostra R[N-1]: %.2f\n", R[N-1]);
    printf("Tempo: %.6f segundos\n", tempo_gasto);

    free(M);
    free(V);
    free(R);
    free(threads);

    return 0;
}