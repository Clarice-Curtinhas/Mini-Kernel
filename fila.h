#ifndef FILA_H
#define FILA_H

#include <stdlib.h>
#include "PCB.h"

typedef struct tFila Fila;

Fila *criaFila(int num_processos);

void adicionaProcessoFila(Fila *fila, PCB *p);

PCB *retiraProcesso(Fila *fila);

int filaVazia(Fila *fila);

void desalocaFilaProcessos(Fila *fila);

void desalocaFilaProntos(Fila *fila);

void imprimeFila(FILE *fp, Fila *fila);

#endif