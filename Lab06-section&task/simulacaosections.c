//
// Created by Felipe Silva on 08/11/26.
//

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define numeroParticulas (1024 * 1024 * 10)
#define passoTempo 0.01

typedef struct {
    double x, y, z;
    double vx, vy, vz;
} particula;

int main() {
    particula *particulas = (particula*) malloc(numeroParticulas * sizeof(particula));
    if (!particulas) {
        perror("malloc falhou");
        return 1;
    }

    long long i;

    printf("Inicializando partículas...\n");
    #pragma omp parallel for
    for (i = 0; i < numeroParticulas; i++) {
        particulas[i].x = (double)rand() / RAND_MAX * 100.0;
        particulas[i].y = (double)rand() / RAND_MAX * 100.0;
        particulas[i].z = (double)rand() / RAND_MAX * 100.0;
        particulas[i].vx = (double)rand() / RAND_MAX * 10.0 - 5.0;
        particulas[i].vy = (double)rand() / RAND_MAX * 10.0 - 5.0;
        particulas[i].vz = (double)rand() / RAND_MAX * 10.0 - 5.0;
    }
    printf("Partículas inicializadas.\n");

    double tempoInicio, tempoFim;
    tempoInicio = omp_get_wtime();

    printf("Simulando um passo (versão com sections)...\n");
    const double gravidade = -9.81;

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            long long indicePosicao;
            for (indicePosicao = 0; indicePosicao < numeroParticulas; indicePosicao++) {
                particulas[indicePosicao].x += particulas[indicePosicao].vx * passoTempo;
                particulas[indicePosicao].y += particulas[indicePosicao].vy * passoTempo;
                particulas[indicePosicao].z += particulas[indicePosicao].vz * passoTempo;
            }
        }

        #pragma omp section
        {
            long long indiceVelocidade;
            for (indiceVelocidade = 0; indiceVelocidade < numeroParticulas; indiceVelocidade++) {
                particulas[indiceVelocidade].vy += gravidade * passoTempo;
            }
        }
    }
    printf("  Posições e velocidades atualizadas.\n");

    tempoFim = omp_get_wtime();

    printf("\nSimulação concluída.\n");
    printf("Tempo de execução (simulação): %.4f segundos\n", tempoFim - tempoInicio);

    free(particulas);
    return 0;
}