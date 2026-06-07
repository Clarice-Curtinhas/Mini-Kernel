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

PCB** getVetor(Fila *fila){
    return fila->fila;
}

int getTam(Fila *fila){
    return fila->tam;
}

int adicionaProcessoFila(Fila *fila, PCB *p){

    if(verificaSeExiste(fila, p) == 1) return 0;
    
    if(fila->tam == fila->cap) return 0;

    fila->fila[fila->final] = p;
    fila->final++;
    fila->final = fila->final % fila->cap;
    fila->tam++;

    return 1;
}

int verificaSeExiste(Fila *fila, PCB *p){
    
    int i = fila->inicial;

    for(int count = 0; count < fila->tam; count++){

        if(fila->fila[i] == p) return 1;

        i++;
    }

    return 0;
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

PCB* getProcesso(Fila *fila, int i){
    
    i = i%fila->cap;
    return fila->fila[i];
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