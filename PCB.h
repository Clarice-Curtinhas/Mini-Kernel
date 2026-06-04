#ifndef PCB_H
#define PCB_H

#include <stdio.h>
#include <stdlib.h>

typedef struct tPCB PCB;

PCB* criaProcesso(int duracao, int prioridade, int n_threads, int tempo_chegada, int pid);

int getPid(PCB *processo);

void imprimeProcesso(FILE *fp, PCB *processo);

void desalocaProcesso(PCB *processo);

#endif