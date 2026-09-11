/*
Kauê Lima Rodrigues Meneses - 10410594

Escreva um programa que use memória compartilhada para inverter uma palavra fornecida pelo usuário.
*/

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

int main(){

    // estrutura que será armazenada na memória compartilhada
    struct dados {
        char palavra[50];
    };

    struct dados dados;

    // leitura da palavra
    printf("Digite uma palavra: ");
    scanf("%49s", dados.palavra);

    // criação da memoria compartilhada
    int shmid = shmget(IPC_PRIVATE, sizeof(struct dados), 0666);

    if(shmid == -1){
        perror("Shared memory");
        return 1;
    }

    // anexa a memoria compartilhada ao processo
    struct dados *shmp = shmat(shmid, NULL, 0);

    if(shmp == (void *) -1){
        perror("Shared memory attach");
        return 1;
    }

    // grava a palavra na memoria compartilhada
    strcpy(shmp->palavra, dados.palavra);

    // cria processo filho para inverter a palavra
    pid_t pid = fork();

    if(pid == -1){
        perror("Fork");
        return 1;

    }else if(pid == 0){

        // processo filho: inverte a palavra
        int tam = strlen(shmp->palavra);

        for(int i = 0; i < tam / 2; i++){
            char temp = shmp->palavra[i];
            shmp->palavra[i] = shmp->palavra[tam - 1 - i];
            shmp->palavra[tam - 1 - i] = temp;
        }

    }else{

        // processo pai espera o filho terminar
        wait(NULL);
    }

    if(pid > 0){

        // processo pai imprime a palavra invertida
        printf("Palavra invertida: %s\n", shmp->palavra);

        // desanexa a memoria compartilhada
        if(shmdt(shmp) == -1){
            perror("Shared memory detach");
            return 1;
        }

        // remove a memoria compartilhada
        if(shmctl(shmid, IPC_RMID, NULL) == -1){
            perror("Shared memory remove");
            return 1;
        }
    }

    return 0;
}
