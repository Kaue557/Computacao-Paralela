// Kauê Lima Rodrigues Meneses - 10410594
/*
  Exercício 2 - Somatório de um vetor grande com OpenMP (critical x reduction)
 
  Compilar: gcc -fopenmp -O2 soma_omp.c -o soma_omp
  Executar: ./soma_omp <N> <threads> <versao> [repeticoes=10]
    versao 1: variável compartilhada + #pragma omp critical
    versao 2: #pragma omp for ... reduction(+: soma)
  Exemplo: ./soma_omp 1073741824 4 2     (N = 2^30 elementos)
 
  O vetor é sempre o mesmo: v[i] = i % 100  -> o resultado é previsível
  e conferido com uma fórmula fechada ao final.
 */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Uso: %s <N> <threads> <versao 1|2> [repeticoes]\n", argv[0]);
        return 1;
    }
    long long N       = strtoll(argv[1], NULL, 10);
    int  threads      = atoi(argv[2]);
    int  versao       = atoi(argv[3]);
    int  reps         = (argc > 4) ? atoi(argv[4]) : 10;

    int *v = malloc(N * sizeof(int));
    if (!v) { fprintf(stderr, "Memoria insuficiente para N = %lld\n", N); return 1; }

    /* Preenchimento (fora da medição de tempo). Mesmos valores sempre. */
#   pragma omp parallel for num_threads(threads)
    for (long long i = 0; i < N; i++)
        v[i] = (int)(i % 100);

    /* Resultado esperado: blocos completos de 100 somam 4950; sobra r elementos (0..r-1) */
    long long esperado = (N / 100) * 4950LL + (N % 100) * (N % 100 - 1) / 2;

    double total = 0.0;
    long long soma = 0;
    for (int r = 0; r < reps; r++) {
        soma = 0;
        double t0 = omp_get_wtime();

        if (versao == 1) {
            /* Cada thread soma o seu pedaço numa variável PRIVADA (parcial);
               só a soma final de cada thread entra na seção crítica. */
#           pragma omp parallel num_threads(threads)
            {
                long long parcial = 0; // private: declarada dentro do bloco
#               pragma omp for
                for (long long i = 0; i < N; i++)
                    parcial += v[i];
#               pragma omp critical
                soma += parcial; // shared: uma thread por vez, por isso o critical em cima
            }
        } else {
#           pragma omp parallel for num_threads(threads) reduction(+: soma)
            for (long long i = 0; i < N; i++)
                soma += v[i];
        }

        double t = omp_get_wtime() - t0;
        total += t;
        printf("execucao %2d: %.4f s\n", r + 1, t);
    }

    printf("soma = %lld (%s)\n", soma, soma == esperado ? "correta" : "ERRADA");
    printf("CSV,%d,%d,%lld,%.6f\n", versao, threads, N, total / reps);  /* linha para o script */
    free(v);
    return 0;
}