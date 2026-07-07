/*
 * main_multi.c
 *
 *  Created on: 04/06/2026
 *      Author: Clarice e Maria Julia
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "escalonador.h"
#include "PCB.h"

#define QNT_PROCESSADORES 2

int main(int argc, char *argv[]){

    if(argc < 1){
        printf("ERRO: quantidade de argumentos insuficiente\n");
        exit(1);
    }

    FILE *entrada = fopen(argv[1], "r");

    Escalonador *escalonador;
    
    int n_processos = 0, n_threads = 0, prioridade, escalonamento;
    int duracao_processo = 0, tempo_chegada = 0;

    pthread_t thread_esc1, thread_esc2;

    fscanf(entrada, "%d", &n_processos);

    escalonador = criaEscalonador(n_processos, 1);

    for(int i = 0; i < n_processos; i++){

        fscanf(entrada, "%d", &duracao_processo);
        fscanf(entrada, "%d", &prioridade);
        fscanf(entrada, "%d", &n_threads);
        fscanf(entrada, "%d", &tempo_chegada);
        
        PCB* p = criaProcesso(duracao_processo, prioridade, n_threads, tempo_chegada, i + 1, 1);
        adicionaProcesso(escalonador, p);
    }

    fscanf(entrada, "%d", &escalonamento);

    setPolitica(escalonador, escalonamento);

    fclose(entrada);

    thread_esc1 = getThread(escalonador, 0);
    thread_esc2 = getThread(escalonador, 1);

    pthread_create(&thread_esc1, NULL, executaEscalonamento, escalonador);
    pthread_create(&thread_esc2, NULL, executaEscalonamento, escalonador);
    pthread_join(thread_esc1, NULL);
    //printf("#### Thread 0 terminou ####\n");
    pthread_join(thread_esc2, NULL);
    //printf("#### Thread 1 terminou ####\n");

    terminaExecucaoBuffer(escalonador);

    FILE *saida = fopen("log_execucao_minikernel.txt", "w");

    imprimeBuffer(escalonador, saida);
    fclose(saida);

    liberaEscalonador(escalonador);

    return 0;
}