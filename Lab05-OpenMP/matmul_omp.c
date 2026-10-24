// Kauê Lima Rodrigues Meneses - 10410594
/*
  Exercício 3 - Multiplicação de matrizes: versão PARALELA com OpenMP
 
  Compilar: gcc -fopenmp -O2 matmul_omp.c -o matmul_omp
  Executar: ./matmul_omp <N> <threads> [repeticoes=3]
 
  Mesmo algoritmo da versão sequencial (laços i-k-j + blocagem BS x BS),
  com OpenMP distribuindo os BLOCOS DE C entre as threads.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#ifndef BS
#define BS 128        /* mesmo tamanho de bloco da versão sequencial */
#endif
#define MIN(a, b) ((a) < (b) ? (a) : (b))

int main(int argc, char *argv[]) {
    if (argc < 3) { fprintf(stderr, "Uso: %s <N> <threads> [repeticoes]\n", argv[0]); return 1; }
    long N      = strtol(argv[1], NULL, 10);
    int threads = atoi(argv[2]);
    int reps    = (argc > 3) ? atoi(argv[3]) : 3;

    double *A = malloc(N * N * sizeof(double));
    double *B = malloc(N * N * sizeof(double));
    double *C = malloc(N * N * sizeof(double));
    if (!A || !B || !C) { fprintf(stderr, "Memoria insuficiente\n"); return 1; }

    for (long i = 0; i < N; i++)
        for (long j = 0; j < N; j++) {
            A[i * N + j] = (double)((i + j) % 10);
            B[i * N + j] = (double)((i * 3 + j) % 7);
        }

    double total = 0.0;
    for (int r = 0; r < reps; r++) {
        for (long i = 0; i < N * N; i++) C[i] = 0.0;

        double t0 = omp_get_wtime();
        /* Cada par (ii,jj) é um bloco de C. collapse(2) junta os dois laços externos
           num só espaço de (N/BS)^2 iterações para repartir entre as threads.
           Cada thread escreve só nos seus blocos de C -> sem condição de corrida,
           sem critical e sem reduction. */
#       pragma omp parallel for collapse(2) schedule(static) num_threads(threads)
        for (long ii = 0; ii < N; ii += BS)
            for (long jj = 0; jj < N; jj += BS)
                for (long kk = 0; kk < N; kk += BS)
                    for (long i = ii; i < MIN(ii + BS, N); i++)
                        for (long k = kk; k < MIN(kk + BS, N); k++) {
                            double aik = A[i * N + k];
                            for (long j = jj; j < MIN(jj + BS, N); j++)
                                C[i * N + j] += aik * B[k * N + j];
                        }
        double t = omp_get_wtime() - t0;
        total += t;
        printf("execucao %d: %.4f s\n", r + 1, t);
    }

    int ok = 1;
    for (int s = 0; s < 8; s++) {
        long i = (s * 7919L) % N, j = (s * 104729L) % N;
        double ref = 0.0;
        for (long k = 0; k < N; k++) ref += A[i * N + k] * B[k * N + j];
        if (fabs(ref - C[i * N + j]) > 1e-6 * fabs(ref) + 1e-9) ok = 0;
    }
    double check = 0.0;
    for (long i = 0; i < N * N; i++) check += C[i];
    printf("checksum = %.1f (%s)\n", check, ok ? "correto" : "ERRADO");
    printf("CSV,omp,%d,%ld,%.6f\n", threads, N, total / reps);

    free(A); free(B); free(C);
    return 0;
}
