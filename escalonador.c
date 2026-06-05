#include "escalonador.h"

#define FCFS 1
#define RR 2
#define PP 3

typedef struct tEscalonador {

    int qtd_processos;
    double quantum;  //Quantum usado no Round Robin (ms) 
    char *log_buffer;  //Armazena  mensagens  de  log  durante  a execução
    Fila *pcb_list; //Lista de todos os processos do sistema
    Fila *fila_prontos; //Fila de prontos circular 
    int generator_done; //Sinaliza que todos os processos foram criados e enfileirados 
    PCB *current_process; //Processo atualmente em execução 
    int scheduler_type; //Define política de escalonamento 
} Escalonador;

Escalonador* criaEscalonador(int qtd_processos){

    Escalonador *e = malloc(sizeof(Escalonador));

    e->qtd_processos = qtd_processos;

    e->pcb_list = criaFila(qtd_processos);
    e->fila_prontos = criaFila(qtd_processos);
    
    e->quantum = 500;
    e->current_process = NULL;
    e->scheduler_type = 0;

    return e;
}

void setPolitica(Escalonador *e, int escalonamento){
    e->scheduler_type = escalonamento;
}

Fila* getFila(Escalonador *e){
    return e->pcb_list;
}

void adicionaProcesso(Escalonador *e, PCB *p){

    adicionaProcessoFila(e->pcb_list, p);
    adicionaProcessoFila(e->fila_prontos, p);
    e->qtd_processos++;
}

void liberaEscalonador(Escalonador *e){

    desalocaFilaProcessos(e->pcb_list);
    desalocaFilaProntos(e->fila_prontos);
    free(e);
}