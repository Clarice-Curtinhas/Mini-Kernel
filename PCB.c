#include "PCB.h"

#define READY 'p'
#define RUNNING 'r'
#define FINISHED 'f'

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

PCB* criaProcesso(int duracao, int prioridade, int n_threads, int tempo_chegada, int pid){

    PCB *processo = malloc(sizeof(PCB));

    processo->pid = pid;
    processo->process_len = duracao;
    processo->remaining_time = duracao;
    processo->priority = prioridade;
    processo->num_threads = n_threads;
    processo->start_time = tempo_chegada;
    processo->state = READY;  ///LEMBRAR DE ALTERAR PARA TESTES

    pthread_mutex_init(&processo->mutex, NULL);
    pthread_cond_init(&processo->cv, NULL);
    
    processo->thread_ids = malloc(processo->num_threads * sizeof(pthread_t));

    for(int i = 0; i < processo->num_threads; i++){
        pthread_create(&processo->thread_ids[i], NULL, thr_func, processo);
    }

    return processo;
}

ProcessState getState(PCB *processo){
    return processo->state;
}

pthread_mutex_t getMutex(PCB *processo){
    return processo->mutex;
}

pthread_cond_t getCondicional(PCB *processo){
    return processo->cv;
}

void diminuiRemainingTime(PCB *processo, int valor){

    processo->remaining_time -= valor;

    if(processo->remaining_time <= 0){
        processo->remaining_time = 0;
        processo->state = FINISHED;
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

    pthread_mutex_destroy(&processo->mutex);
    pthread_cond_destroy(&processo->cv);
    
    for(int i = 0; i < processo->num_threads; i++){
        if(pthread_join(processo->thread_ids[i], NULL) != 0){
            printf("ERRO: Pthread join falhou!\n");
        }
    }
    
    free(processo->thread_ids);
    free(processo);
}