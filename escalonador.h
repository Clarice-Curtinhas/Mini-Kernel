#ifndef ESCALONADOR_H
#define ESCALONADOR_H

#include "PCB.h"
#include "fila.h"

#include <stdio.h>
#include <string.h>
#include <pthread.h>

typedef struct tEscalonador Escalonador;

Escalonador* criaEscalonador(int qtd_processos);

Fila* getFila(Escalonador *e);

void setPolitica(Escalonador *e, int escalonamento);

void *executaEscalonamento(void *arg);

void escalonamentoFCFS(Escalonador *e);

void escalonamentoRR(Escalonador *e);

void escalonamentoPP(Escalonador *e);

void realocaBuffer(Escalonador *e, int test);

void verificaProcessosValidos(Escalonador *e);

void terminaExecucaoBuffer(Escalonador *e);

void finalizaPcbBuffer(Escalonador *e);

void executaPcbBuffer(Escalonador *e);

void adicionaProcesso(Escalonador *e, PCB *p);

void imprimeBuffer(Escalonador *e, FILE *fp);

void liberaEscalonador(Escalonador *e);

#endif