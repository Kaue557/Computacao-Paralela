#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <N> <Bloco B>\n", argv[0]);
        return 1;
    }
    
    int N = atoi(argv[1]);
    int bloco_B = atoi(argv[2]);

    double *A = (double*) malloc(N * N * sizeof(double));
    double *B = (double*) malloc(N * N * sizeof(double));
    double *C = (double*) calloc(N * N, sizeof(double));

    // Inicialização determinística padronizada
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i * N + j] = (double)(i + j);
            B[i * N + j] = (double)(i * j);
        }
    }

    struct timespec inicio, fim;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    // Algoritmo com Blocagem (Tiling) e reordenação (i, k, j) para SIMD
    for (int ii = 0; ii < N; ii += bloco_B) {
        for (int jj = 0; jj < N; jj += bloco_B) {
            for (int kk = 0; kk < N; kk += bloco_B) {
                
                // Multiplicação do bloco B x B
                for (int i = ii; i < ii + bloco_B && i < N; i++) {
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

    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempo = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    printf("[Matmul Blocado] N: %d | Bloco B: %d | Amostra C[N-1]: %.2f | Tempo: %.4f s\n", N, bloco_B, C[N * N - 1], tempo);

    free(A);
    free(B);
    free(C);

    return 0;
}