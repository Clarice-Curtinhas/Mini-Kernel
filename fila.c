#include "fila.h"

typedef struct tFila {
    PCB **fila;
    int inicial;
    int final;
    int cap;
    int tam;
} Fila;

Fila *criaFila(int num_processos){

    Fila *f = malloc(num_processos * sizeof(Fila));
    
    f->fila = malloc(num_processos * sizeof(PCB*));
    f->inicial = f->final = 0;
    f->cap = num_processos;
    f->tam = 0;
    
    return f;
}

void adicionaProcessoFila(Fila *fila, PCB *p){
    
    if(fila->tam == fila->cap) return;

    fila->fila[fila->final] = p;
    fila->final++;
    fila->final = fila->final % fila->cap;
    fila->tam++;
}

int filaVazia(Fila *fila){

    if(fila->tam == 0){
        return 1;
    }
    return 0;
}

PCB *retiraProcesso(Fila *fila){

    if(fila->tam == 0){
        return NULL;
    }

    PCB *p = fila->fila[fila->inicial];
    fila->inicial++;
    
    fila->inicial = fila->inicial % fila->cap;
    fila->tam--;

    return p;
}

void desalocaFilaProcessos(Fila *fila){

    for(int i = 0; i < fila->cap; i++){
        desalocaProcesso(fila->fila[i]);
    }

    free(fila->fila);
    free(fila);
}

void desalocaFilaProntos(Fila *fila){

    free(fila->fila);
    free(fila);
}

void imprimeFila(FILE *fp, Fila *fila){
    
    for(int i = 0; i != fila->cap; i++){
        imprimeProcesso(fp, fila->fila[i]);
    }
}