/*
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

int main(){
// leitura da palavra
char palavra[50];

printf("Confira se uma palavra eh palíndromo: ");
scanf("%s", palavra);

// criação da memoria compartilhada
int shmid = shmget(IPC_PRIVATE, sizeof(palavra), 0666);
if(shmid == -1){
    perror("Shared memory");
    return 1;
}
// (!) NAO eh necessario IPC_CREAT com o uso do IPC_PRIVATE,
// pois a memoria compartilhada eh criada automaticamente

char *shmp = shmat(shmid, NULL, 0); // char * apontando para o início do espaço compartilhado.
if(shmp == (void *) -1){
    perror("Shared memory attach");
    return 1;
}


// "grava" a palavra nela
strcpy(shmp, palavra);


// verifica se a palavra eh palindromo
int tam = strlen(shmp);
bool ehPalindromo = true;

for(int i = 0; i < tam / 2; i++){
        if(shmp[i] != shmp[tam - 1 -i]){
            ehPalindromo = false;
            break;
        }
}

if(ehPalindromo){
    printf("A palavra %s eh palindromo\n", shmp);
}else{
    printf("A palavra %s nao eh palindromo\n", shmp);
}

return 0;
}