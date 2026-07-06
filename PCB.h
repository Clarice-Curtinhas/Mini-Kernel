#ifndef PCB_H
#define PCB_H

#include <stdio.h>
#include <stdlib.h>
#include "TCB.h"

#define READY 'p'
#define RUNNING 'r'
#define FINISHED 'f'

typedef char ProcessState;

typedef struct tPCB PCB;

PCB* criaProcesso(int duracao, int prioridade, int n_threads, int tempo_chegada, int pid, int tipo_processador);

ProcessState getState(PCB *processo);

void setState(PCB *processo, ProcessState novoEstado);

void resetThreadsQuantum(PCB *processo);

void setThreadsQuantum(PCB *processo);

int getThreadsQuantum(PCB *processo);

int getPid(PCB *processo);

int getDuracao(PCB *processo);

void incrementaProcessadores(PCB *processo);

void decrementaProcessadores(PCB *processo);

int getProcessadoresExecutando(PCB *processo);

int getQuantumProcesso(PCB *processo);

int getThreadsRestantes(PCB *processo);

void setFinalizado(PCB *processo);

int getFinalizado(PCB *processo);

void setThreadsRestantes(PCB *processo);

int getTipoProcessador(PCB *processo);

int getRemainingTime(PCB *processo);

int getNumThreads(PCB *processo);

int getTempoChegada(PCB *processo);

int getPrioridade(PCB *processo);

int getTipoEscalonamento(PCB *processo);

void setTipoEscalonamento(PCB *processo, int tipo);

int comparaProcessos(const void *a, const void *b);

pthread_mutex_t* getMutex(PCB *processo);

pthread_cond_t* getCondicional(PCB *processo);

void diminuiRemainingTime(PCB *processo, int valor);

void imprimeProcesso(FILE *fp, PCB *processo);

void desalocaProcesso(PCB *processo);

#endif