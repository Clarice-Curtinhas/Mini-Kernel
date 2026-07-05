#include "PCB.h"

#define FCFS 1
#define RR 2
#define PP 3

typedef struct tPCB {
    int pid; 
    int process_len; 
    int remaining_time; 
    int priority; 
    int num_threads; 
    int start_time; 
    int tipo_escalonamento;
    int tipo_processador;
    int threads_restantes;
    ProcessState state; 
    pthread_mutex_t mutex; 
    pthread_cond_t cv; 
    pthread_t *thread_ids; 
} PCB;

int comparaProcessos(const void *a, const void *b){

    PCB *p1 = *(PCB**) a;
    PCB *p2 = *(PCB**) b;

    return getTempoChegada(p1) - getTempoChegada(p2);
}

PCB* criaProcesso(int duracao, int prioridade, int n_threads, int tempo_chegada, int pid, int tipo_processador){

    PCB *processo = malloc(sizeof(PCB));

    processo->pid = pid;
    processo->process_len = duracao;
    processo->remaining_time = duracao;
    processo->priority = prioridade;
    processo->num_threads = n_threads;
    processo->start_time = tempo_chegada;
    processo->state = READY;  ///LEMBRAR DE ALTERAR PARA TESTES
    processo->tipo_escalonamento = 0;
    processo->tipo_processador = tipo_processador;
    processo->threads_restantes = n_threads;

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

void setThreadsRestantes(PCB *processo){
    processo->threads_restantes--;
}

int getRemainingTime(PCB *processo){
    return processo->remaining_time;
}

int getThreadsExecutadas(PCB *processo){
    return processo->threads_restantes;
}

int getQuantumProcesso(PCB *processo){
    return getDuracao(processo)/getNumThreads(processo);
}

int getTipoProcessador(PCB *processo){
    return processo->tipo_processador;
}

void setState(PCB *processo, ProcessState novoEstado){
    processo->state = novoEstado;
}

pthread_mutex_t* getMutex(PCB *processo){
    return &processo->mutex;
}

int getDuracao(PCB *processo){
    return processo->process_len;
}

int getTempoChegada(PCB *processo){
    return processo->start_time;
}

int getNumThreads(PCB *processo){
    return processo->num_threads;
}

int getPrioridade(PCB *processo){
    return processo->priority;
}

pthread_cond_t* getCondicional(PCB *processo){
    return &processo->cv;
}

int getPid(PCB *processo){
    return processo->pid;
}

int getTipoEscalonamento(PCB *processo){
    return processo->tipo_escalonamento;
}

void setTipoEscalonamento(PCB *processo, int tipo){
    processo->tipo_escalonamento = tipo;
}

void diminuiRemainingTime(PCB *processo, int valor){

    processo->remaining_time -= valor;

    if(processo->remaining_time <= 0){
        processo->remaining_time = 0;
        processo->state = FINISHED;
        printf("Processo %d foi finished\n", processo->pid);
    }
    
    pthread_cond_t *cond = getCondicional(processo);
    pthread_cond_broadcast(cond);
}

void imprimeProcesso(FILE *fp, PCB *processo){

    if(fp != NULL){
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

    else{
        printf("\nProcess PID: %d\n", processo->pid);

        printf("   número de threads: %d\n", processo->num_threads);

        for(int i = 0; i < processo->num_threads; i++){
            printf("       thread: %d\n", i);
        }

        printf("   tamanho do processo: %d\n", processo->process_len);
        printf("   tempo restante: %d\n", processo->remaining_time);
        
        printf("   prioridade: %d\n", processo->priority);

        if(processo->state == READY) printf("   estado: READY\n");
        else if(processo->state == RUNNING) printf("   estado: RUNNING\n");
        else printf("   estado: FINISHED\n");
    }
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