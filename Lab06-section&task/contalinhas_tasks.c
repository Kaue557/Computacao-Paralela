//
// Created by thesposito on 08/11/2026.
//
#define _GNU_SOURCE
#define _XOPEN_SOURCE 700 // Para scandir, strdup
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> // Para operações de diretório
#include <sys/stat.h> // Para stat
#include <errno.h>
#include <omp.h> // Usaremos para timing inicialmente

// Contador global compartilhado por todas as tarefas.
// Cada tarefa de arquivo soma seu resultado aqui usando #pragma omp atomic.
long long total_lines_global = 0;

// Função para contar linhas em um único arquivo
long long count_lines_in_file(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        // fprintf(stderr, "Erro ao abrir arquivo %s: %s\n", filepath, strerror(errno));
        return 0; // Ignora arquivos que não pode abrir
    }
    long long count = 0;
    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\n') {
            count++;
        }
    }
    fclose(fp);
    // Adiciona 1 se o arquivo não terminar com newline mas não for vazio
    // Simplificação: vamos apenas contar os '\n'
    return count;
}

// Versão com tasks: em vez de retornar a soma, cada tarefa de arquivo
// atualiza o contador global total_lines_global de forma atômica.
void traverse_directory(const char *dirpath) {
    struct dirent **namelist;
    int n;
    n = scandir(dirpath, &namelist, NULL, alphasort);
    if (n < 0) {
        fprintf(stderr, "Erro ao escanear diretório %s: %s\n", dirpath, strerror(errno));
        return;
    }
    // printf("Entrando em %s (%d itens)\n", dirpath, n);
    for (int i = 0; i < n; i++) {
        // Ignora "." e ".."
        if (strcmp(namelist[i]->d_name, ".") == 0
        || strcmp(namelist[i]->d_name, "..") == 0) {
            free(namelist[i]);
            continue;
        }
        // Monta o caminho completo
        char *fullpath = NULL;
        // Calcula tamanho necessário +2 para '/' e '\0'
        if (asprintf(&fullpath, "%s/%s", dirpath, namelist[i]->d_name) == -1) {
            fprintf(stderr, "Erro de alocação em asprintf\n");
            free(namelist[i]);
            continue; // Pula este item
        }

        struct stat path_stat;
        if (stat(fullpath, &path_stat) != 0) {
            fprintf(stderr, "Erro no stat para %s: %s\n", fullpath, strerror(errno));
            free(fullpath);
            free(namelist[i]);
            continue;
        }
        // Se for um diretório, chama recursivamente
        // (agora dentro de uma task: outra thread pode explorar o subdiretório)
        if (S_ISDIR(path_stat.st_mode)) {
            // fullpath é firstprivate: a task recebe sua própria cópia do ponteiro
            // e fica responsável por liberar a memória quando terminar.
            #pragma omp task firstprivate(fullpath)
            {
                traverse_directory(fullpath);
                free(fullpath);
            }
        }
        // Se for um arquivo regular e termina com .c ou .h
        else if (S_ISREG(path_stat.st_mode)) {
            size_t len = strlen(namelist[i]->d_name);
            if (len > 2 && (strcmp(namelist[i]->d_name + len - 2, ".c") == 0 ||
            strcmp(namelist[i]->d_name + len - 2, ".h") == 0))
            {
                // printf(" Processando arquivo: %s\n", fullpath);
                // Cria uma task para contar as linhas do arquivo
                #pragma omp task firstprivate(fullpath)
                {
                    long long lines = count_lines_in_file(fullpath);
                    // Atualização atômica do contador global (evita condição de corrida)
                    #pragma omp atomic
                    total_lines_global += lines;
                    free(fullpath);
                }
            } else {
                free(fullpath); // Arquivo regular que não é .c/.h
            }
        } else {
            free(fullpath); // Nem diretório nem arquivo regular
        }
        // fullpath NÃO é liberado aqui quando uma task foi criada:
        // a task ainda pode estar usando o caminho.
        free(namelist[i]);
    }
    free(namelist);
    // Não é preciso taskwait aqui: ninguém usa um valor de retorno desta função,
    // e a barreira implícita no fim da região parallel espera todas as tasks.

    // printf("Saindo de %s\n", dirpath);
}

int main(int argc, char *argv[]) {
    const char *start_dir = "."; // Diretório atual por padrão
    if (argc > 1) {
        start_dir = argv[1];
    }
    printf("Contando linhas em .c/.h a partir de: %s (versão paralela com tasks)\n",
    start_dir);
    double start_time, end_time;
    start_time = omp_get_wtime();
    // --- Bloco a ser otimizado ---
    #pragma omp parallel
    {
        // Apenas uma thread inicia a travessia; as tasks criadas por ela
        // (e pelas tasks filhas) são executadas por todas as threads da equipe.
        #pragma omp single
        {
            traverse_directory(start_dir);
        }
    } // Barreira implícita: aqui todas as tasks já terminaram
    long long total_lines = total_lines_global;
    // --- Fim do Bloco ---
    end_time = omp_get_wtime();
    printf("\n--- Resultado ---\n");
    printf("Total de linhas encontradas: %lld\n", total_lines);
    printf("Tempo de execução: %.4f segundos\n", end_time - start_time);

    return 0;
}