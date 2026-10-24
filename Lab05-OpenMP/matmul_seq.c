// Kauê Lima Rodrigues Meneses - 10410594
/*
  Exercício 3 - Multiplicação de matrizes: versão SEQUENCIAL (com blocagem para cache)
 
  Compilar: gcc -O2 matmul_seq.c -o matmul_seq
  Executar: ./matmul_seq <N> [repeticoes=3]        (matrizes N x N de double)
 
  Técnicas de cache usadas:
   1) Ordem de laços i-k-j: o laço mais interno percorre B e C ao longo da LINHA
      (endereços consecutivos), aproveitando as linhas de cache. A ordem i-j-k
      percorreria B por coluna, com saltos de N*8 bytes (um miss de cache por acesso).
   2) Blocagem (tiling) em blocos de BS x BS: o bloco de B reutilizado cabe em L1/L2.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifndef BS
#define BS 128        /* bloco: 3 blocos de 128x128 doubles = 384 KB (cabem em L2); teste -DBS=64/256 */
#endif
#define MIN(a, b) ((a) < (b) ? (a) : (b))

/* relógio de parede em segundos (sem depender do OpenMP) */
static double wtime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(int argc, char *argv[]) {
    if (argc < 2) { fprintf(stderr, "Uso: %s <N> [repeticoes]\n", argv[0]); return 1; }
    long N   = strtol(argv[1], NULL, 10);
    int reps = (argc > 2) ? atoi(argv[2]) : 3;

    double *A = malloc(N * N * sizeof(double));
    double *B = malloc(N * N * sizeof(double));
    double *C = malloc(N * N * sizeof(double));
    if (!A || !B || !C) { fprintf(stderr, "Memoria insuficiente\n"); return 1; }

    /* Mesmos valores sempre (resultado reprodutível) */
    for (long i = 0; i < N; i++)
        for (long j = 0; j < N; j++) {
            A[i * N + j] = (double)((i + j) % 10);
            B[i * N + j] = (double)((i * 3 + j) % 7);
        }

    double total = 0.0;
    for (int r = 0; r < reps; r++) {
        for (long i = 0; i < N * N; i++) C[i] = 0.0;   /* zera C (fora da medição) */

        double t0 = wtime();
        for (long ii = 0; ii < N; ii += BS)
            for (long jj = 0; jj < N; jj += BS)
                for (long kk = 0; kk < N; kk += BS)
                    /* multiplica o bloco A[ii,kk] pelo bloco B[kk,jj] e acumula em C[ii,jj] */
                    for (long i = ii; i < MIN(ii + BS, N); i++)
                        for (long k = kk; k < MIN(kk + BS, N); k++) {
                            double aik = A[i * N + k];
                            for (long j = jj; j < MIN(jj + BS, N); j++)
                                C[i * N + j] += aik * B[k * N + j];
                        }
        double t = wtime() - t0;
        total += t;
        printf("execucao %d: %.4f s\n", r + 1, t);
    }

    /* Verificação: 8 elementos conferidos com o produto escalar direto + checksum */
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
    printf("CSV,seq,1,%ld,%.6f\n", N, total / reps);

    free(A); free(B); free(C);
    return 0;
}
