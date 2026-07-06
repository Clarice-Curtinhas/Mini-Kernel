#include "fila.h"

typedef struct tFila {
    PCB **fila;
    int inicial;
    int final;
    int cap;
    int tam;
} Fila;

Fila *criaFila(int num_processos){

    Fila *f = (Fila*) calloc(1, sizeof(Fila));
    
    f->fila = (PCB**) calloc(num_processos, sizeof(PCB*));
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

    //printf("Processo %d entrou na fila\n", getPid(p));

    if(verificaSeExiste(fila, p) == 1) return 0;
    
    if(fila->tam == fila->cap) return 0;

    fila->fila[fila->final] = p;
    fila->final++;
    fila->final = fila->final % fila->cap;
    fila->tam++;

    return 1;
}

int verificaSeExiste(Fila *fila, PCB *p){
    
    int i = fila->inicial % fila->cap;

    for(int count = 0; count < fila->tam; count++){

        if(fila->fila[i] == p) return 1;

        i = (i+1) % fila->cap;
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

    fila->fila[fila->inicial] = NULL;

    fila->inicial++;
    fila->inicial = fila->inicial % fila->cap;
    fila->tam--;

    //printf("Retirou algo...%d\n", fila->tam);

    return p;
}

PCB* getPrimeiro(Fila *fila){
    PCB* prim = fila->fila[fila->inicial];

    if((getState(prim) == RUNNING && getNumThreads(prim) == 1) || (getState(prim) == FINISHED) || getRemainingTime(prim) <= 0){
        return fila->fila[fila->inicial+1];
    }

    if(fila->tam == 1){
        return fila->fila[fila->inicial];
    }

    return fila->fila[fila->inicial];
}

PCB* getProcesso(Fila *fila, int i){
    
    i = i%fila->cap;
    return fila->fila[i];
}

PCB* getProcessoMaiorPrioridade(Fila *fila){

    if(fila->tam == 0){
        return NULL;
    }

    PCB *maior = NULL;

    int atual, ind_maior, ind_prox;

    ind_maior = atual = fila->inicial % fila->cap;
    maior = fila->fila[atual];

    //printf("tam da fila: %d\n", fila->tam);

    for(int i = 0; i < fila->tam; i++){
        //printf("Prioridade maior: %d, Prioridade atual: %d\n", getPrioridade(maior), getPrioridade(fila->fila[atual]));
        if(fila->fila[atual] != NULL){
            if(getPrioridade(fila->fila[atual]) < getPrioridade(maior)){
                //printf("maior antigo:%d < agora: %d\n", getPrioridade(maior), getPrioridade(fila->fila[atual]));
                maior = fila->fila[atual];
                ind_maior = atual;
            }
        }

        //printf("pid: %d - prioridade: %d\n", getPid(fila->fila[i]), getPrioridade(fila->fila[i]));

        atual = (atual+1) % fila->cap;
    }

    fila->final = (fila->final - 1 + fila->cap) % fila->cap;

    while(ind_maior != fila->final){
        ind_prox = (ind_maior+1) % fila->cap;
        fila->fila[ind_maior] = fila->fila[ind_prox];
        ind_maior = ind_prox;
    }

    fila->fila[fila->final] = NULL;
    fila->tam--;

    return maior;
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
        if(fila->fila[i] != NULL) imprimeProcesso(fp, fila->fila[i]);
    }
}