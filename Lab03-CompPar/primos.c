#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

long long K;
int num_threads;
long long total_primos = 0;

pthread_mutex_t mutex_primos = PTHREAD_MUTEX_INITIALIZER;

bool eh_primo(long long n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (long long i = 3; i * i <= n; i += 2) { // Verifica apenas números ímpares
        if (n % i == 0) return false;
    }
    return true;
}

void* contar_primos(void* arg) {
    long my_rank = (long)arg;
    long long primos_locais = 0; // Contador local de primos para cada thread, evita acesso concorrente

    for (long long i = 2 + my_rank; i <= K; i += num_threads) {
        if (eh_primo(i)) {
            primos_locais++;
        }
    }

    // Atualiza o total de primos de forma segura usando mutex
    pthread_mutex_lock(&mutex_primos);
    total_primos += primos_locais;
    pthread_mutex_unlock(&mutex_primos);

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Uso: %s <K> <num_threads>\n", argv[0]);
        return 1;
    }

    K = atoll(argv[1]);
    num_threads = atoi(argv[2]);

    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    struct timespec inicio, fim;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (long i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, contar_primos, (void*)i);
    }

    for (long i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &fim);

    double tempo_gasto = (fim.tv_sec - inicio.tv_sec) +
                         (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    printf("Total de primos: %lld\n", total_primos);
    printf("Tempo: %.6f segundos\n", tempo_gasto);

    free(threads);
    pthread_mutex_destroy(&mutex_primos);

    return 0;
}