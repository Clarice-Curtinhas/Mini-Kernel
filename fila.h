#ifndef FILA_H
#define FILA_H

#include <stdlib.h>
#include "PCB.h"

typedef struct tFila Fila;

Fila *criaFila(int num_processos);

int adicionaProcessoFila(Fila *fila, PCB *p);

PCB *retiraProcesso(Fila *fila);

int filaVazia(Fila *fila);

int verificaSeExiste(Fila *fila, PCB *p);

PCB** getVetor(Fila *fila);

PCB* getProcessoMaiorPrioridade(Fila *fila);

int getTam(Fila *fila);

PCB* getPrimeiro(Fila *fila);

PCB* getProcesso(Fila *fila, int i);

void desalocaFilaProcessos(Fila *fila);

void desalocaFilaProntos(Fila *fila);

void imprimeFila(FILE *fp, Fila *fila);

#endif