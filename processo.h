#ifndef BCP_H
#define BCP_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Bcp tBcp;

tProcess* criaProcesso(double duracao, int prioridade, int n_threads, double tempo_chegada, int pid);

void imprimeProcesso(FILE *fp, tProcess *processo);

void desalocaProcesso(tProcess *processo);

#endif