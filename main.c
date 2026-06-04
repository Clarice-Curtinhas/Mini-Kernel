#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "PCB.h"

#define FCFS 1
#define RR 2
#define PP 3

int main(int argc, char *argv[]){

    if(argc < 1){
        printf("ERRO: quantidade de argumentos insuficiente\n");
        exit(1);
    }

    FILE *entrada = fopen(argv[1], "r");
    
    int n_processos = 0, n_threads = 0, prioridade, escalonamento;
    int duracao_processo = 0, tempo_chegada = 0;

    fscanf(entrada, "%d", &n_processos);

    PCB *processo[n_processos];

    for(int i = 0; i < n_processos; i++){

        fscanf(entrada, "%d", &duracao_processo);
        fscanf(entrada, "%d", &prioridade);
        fscanf(entrada, "%d", &n_threads);
        fscanf(entrada, "%d", &tempo_chegada);
        
        processo[i] = criaProcesso(duracao_processo, prioridade, n_threads, tempo_chegada, i);
    }

    fscanf(entrada, "%d", &escalonamento);

    fclose(entrada);

    ///

    FILE *saida = fopen("log_execucao_minikernel.txt", "w");
    
    if(escalonamento == FCFS){
        fprintf(saida, "Escalonamento: FCFS\n");
    }

    if(escalonamento == RR){
        fprintf(saida, "Escalonamento: RR\n");
    }

    if(escalonamento == PP){
        fprintf(saida, "Escalonamento: PP\n");
    }

    for(int i = 0; i < n_processos; i++){
        imprimeProcesso(saida, processo[i]);
        desalocaProcesso(processo[i]);
    }

    fclose(saida);

    return 0;
}