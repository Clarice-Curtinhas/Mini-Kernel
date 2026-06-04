#include "PCB.h"

#define READY 'p'
#define RUNNING 'r'
#define FINISHED 'f'

typedef char ProcessState;

typedef struct tPCB {

    int pid; 
    int process_len; 
    int remaining_time; 
    int priority; 
    int num_threads; 
    int start_time; 

    ProcessState state; 
    
    pthread_mutex_t mutex; 
    pthread_cond_t cv; 
    pthread_t *thread_ids; 
} PCB;

/*
double quantum;  //Quantum usado no Round Robin (ms) 
char *log_buffer;  //Armazena  mensagens  de  log  durante  a execução

PCB *pcb_list; //Lista de todos os processos do sistema
runqueue; //Fila de prontos circular 
bool generator_done; //Sinaliza que todos os processos foram criados e enfileirados 
Process *current_process; //Processo atualmente em execução 
int scheduler_type; //Define política de escalonamento 
*/

PCB* criaProcesso(int duracao, int prioridade, int n_threads, int tempo_chegada, int pid){

    PCB *processo = malloc(sizeof(PCB));

    processo->pid = pid;
    processo->process_len = duracao;
    processo->remaining_time = duracao;
    processo->priority = prioridade;
    processo->num_threads = n_threads;
    processo->start_time = tempo_chegada;
    processo->state = READY;
    processo->thread_ids = NULL;

    return processo;
}

int getPid(PCB *processo){

    return processo->pid;
}

void executaProcesso(PCB *processo){
    
    for(int i = 0; i < processo->n_threads; i++){

        tTcb *thread = criaThread(processo);
        
        if(){
            break;
        }
    }
}

void imprimeProcesso(FILE *fp, PCB *processo){
    fprintf(fp, "\nProcess PID: %d\n", processo->pid);

    fprintf(fp, "   número de threads: %d\n", processo->num_threads);

    for(int i = 0; i < processo->num_threads; i++){
        fprintf(fp, "       thread: %d\n", i);
    }

    fprintf(fp, "   tamanho do processo: %d\n", processo->process_len);
    fprintf(fp, "   tempo restante: %d\n", processo->remaining_time);
    
    fprintf(fp, "   prioridade: %d\n", processo->priority);

    if(processo->state == READY) fprintf(fp, "   estado: READY\n");
    else if(processo->state == RUNNING) fprintf(fp, "   estado: RUNNING\n");
    else fprintf(fp, "   estado: FINISHED\n");
}

void desalocaProcesso(PCB *processo){
    free(processo->thread_ids);
    free(processo);
}