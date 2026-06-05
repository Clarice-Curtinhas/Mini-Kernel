#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "escalonador.h"
#include "PCB.h"

int main(int argc, char *argv[]){

    if(argc < 1){
        printf("ERRO: quantidade de argumentos insuficiente\n");
        exit(1);
    }

    FILE *entrada = fopen(argv[1], "r");

    Escalonador *escalonador;
    
    int n_processos = 0, n_threads = 0, prioridade, escalonamento;
    int duracao_processo = 0, tempo_chegada = 0;

    pthread_t thread_esc;

    fscanf(entrada, "%d", &n_processos);

    escalonador = criaEscalonador(n_processos);

    for(int i = 0; i < n_processos; i++){

        fscanf(entrada, "%d", &duracao_processo);
        fscanf(entrada, "%d", &prioridade);
        fscanf(entrada, "%d", &n_threads);
        fscanf(entrada, "%d", &tempo_chegada);
        
        PCB* p = criaProcesso(duracao_processo, prioridade, n_threads, tempo_chegada, i);
        adicionaProcesso(escalonador, p);
    }

    fscanf(entrada, "%d", &escalonamento);

    setPolitica(escalonador, escalonamento);

    fclose(entrada);

    pthread_create(&thread_esc, NULL, executaEscalonamento, escalonador);
    pthread_join(thread_esc, NULL);

    FILE *saida = fopen("log_execucao_minikernel.txt", "w");

    imprimeBuffer(escalonador, saida);

    liberaEscalonador(escalonador);

    fclose(saida);

    return 0;
}