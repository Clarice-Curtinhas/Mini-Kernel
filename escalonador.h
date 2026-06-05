#ifndef ESCALONADOR_H
#define ESCALONADOR_H

#include "PCB.h"
#include "fila.h"

typedef struct tEscalonador Escalonador;

Escalonador* criaEscalonador(int qtd_processos);
Fila* getFila(Escalonador *e);
void setPolitica(Escalonador *e, int escalonamento);
void adicionaProcesso(Escalonador *e, PCB *p);
void liberaEscalonador(Escalonador *e);

#endif