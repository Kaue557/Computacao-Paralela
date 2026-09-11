/*
Kauê Lima Rodrigues Meneses - 10410594

Escreva um programa que use memória compartilhada para verificar
se uma palavra fornecida pelo usuário é palíndromo ou não.
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
// leitura da palavra
struct dados {
    char palavra[50];
    bool ehPalindromo;
};

struct dados dados;

printf("Confira se uma palavra eh palíndromo: ");
scanf("%s", dados.palavra);

// criação da memoria compartilhada
int shmid = shmget(IPC_PRIVATE, sizeof(struct dados), 0666);
if(shmid == -1){
    perror("Shared memory");
    return 1;
}
// (!) NAO eh necessario IPC_CREAT com o uso do IPC_PRIVATE,
// pois a memoria compartilhada eh criada automaticamente

struct dados *shmp = shmat(shmid, NULL, 0); // struct dados * apontando para o início do espaço compartilhado.
if(shmp == (void *) -1){
    perror("Shared memory attach");
    return 1;
}


// "grava" a palavra nela
strcpy(shmp->palavra, dados.palavra);
shmp->ehPalindromo = true;

// verifica se a palavra eh palindromo
int tam = strlen(shmp->palavra);

// cria um processo filho para fazer a verificação
pid_t pid = fork();

if(pid == -1){
    perror("Fork");
    return 1;
}else if(pid == 0){ // se for o processo filho
    for(int i = 0; i < tam / 2; i++){
        if(shmp->palavra[i] != shmp->palavra[tam - 1 -i]){
            shmp->ehPalindromo = false;
            break;
        }
    }
}else{ // se for o processo pai
    wait(NULL); // espera o filho terminar
}

if(pid > 0){ // se for o processo pai, imprime o resultado
    if(shmp->ehPalindromo){
        printf("A palavra %s eh palindromo\n", shmp->palavra);
    }else{
        printf("A palavra %s nao eh palindromo\n", shmp->palavra);
    }
    
    // libera a memoria compartilhada
    if(shmdt(shmp) == -1){
        perror("Shared memory detach");
        return 1;
    }else{
        printf("Shared memory detached\n");
    }

    // remove a memoria compartilhada
    shmctl(shmid, IPC_RMID, NULL);
}

return 0;
}