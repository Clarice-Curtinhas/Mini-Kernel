#ifndef ESCALONADOR_H
#define ESCALONADOR_H

#include "PCB.h"
#include "fila.h"

#include <stdio.h>
#include <string.h>
#include <pthread.h>

typedef struct tEscalonador Escalonador;

Escalonador* criaEscalonador(int qtd_processos, int tipo_processador);

Fila* getFila(Escalonador *e);

pthread_t getThread(Escalonador *e, int i);

void setPolitica(Escalonador *e, int escalonamento);

void *executaEscalonamento(void *arg);

void FCFS_mono(Escalonador *e);

void FCFS_multi(Escalonador *e);

void RR_mono(Escalonador *e);

void RR_multi(Escalonador *e);

void PP_mono(Escalonador *e);

void PP_multi(Escalonador *e);

void realocaBuffer(Escalonador *e, int test);

void verificaProcessosValidos(Escalonador *e);

void terminaExecucaoBuffer(Escalonador *e);

void finalizaPcbBuffer(Escalonador *e);

void executaPcbBuffer(Escalonador *e);

void adicionaProcesso(Escalonador *e, PCB *p);

void imprimeBuffer(Escalonador *e, FILE *fp);

void liberaEscalonador(Escalonador *e);

#endif