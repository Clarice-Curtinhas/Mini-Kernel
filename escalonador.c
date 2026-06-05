#include "escalonador.h"

#define FCFS 1
#define RR 2
#define PP 3

#define TAM_BUFFER 250

typedef struct tEscalonador {

    int qtd_processos;
    double quantum;  //Quantum usado no Round Robin (ms) 
    char *log_buffer;  //Armazena  mensagens  de  log  durante  a execução
    int tam_buffer;
    int final_buffer;
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

    e->log_buffer = malloc(TAM_BUFFER * sizeof(char)); 
    e->log_buffer[0] = '\0';
    e->tam_buffer = TAM_BUFFER;

    e->quantum = 500;
    e->final_buffer = 0;
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

void *executaEscalonamento(void *arg){

    Escalonador *e = (Escalonador*)arg;
    
    if(e->scheduler_type == FCFS){
        escalonamentoFCFS(e);
    }

    /*else if(e->scheduler_type == RR){
        escalonamentoRR(e);
    }

    else{
        escalonamentoPP(e);
    }*/
}

void escalonamentoFCFS(Escalonador *e){

    char frase[60];

    while(filaVazia(e->fila_prontos) == 0){
        
        if(e->current_process == NULL || getState(e->current_process) == FINISHED){
            ///////
            if(e->tam_buffer < e->final_buffer+70){
                e->log_buffer = realloc(e->log_buffer, (e->tam_buffer+TAM_BUFFER) * sizeof(char));
                e->tam_buffer += TAM_BUFFER;
            }
            
            if(e->current_process != NULL){
                snprintf(frase, sizeof(frase), "[FCFS] Processo PID %d finalizado\n", getPid(e->current_process));
                strcat(e->log_buffer, frase);
                e->final_buffer += sizeof(frase);
            }
            ///////
            
            PCB *p = retiraProcesso(e->fila_prontos);

            printf("Processo %d retirado da fila\n", getPid(p));

            e->current_process = p;

            pthread_mutex_t *mutex = getMutex(p);
            pthread_cond_t *cond = getCondicional(p);

            pthread_mutex_lock(mutex);

            setState(p, RUNNING);
            printf("Processo PID: %d running\n", getPid(p));
            pthread_cond_broadcast(cond);

            while(getState(p) != FINISHED){
                pthread_cond_wait(cond, mutex);
            }

            ///////
            printf("Processo PID: %d broadcast\n", getPid(e->current_process));
            
            pthread_mutex_unlock(mutex);

            snprintf(frase, sizeof(frase), "[FCFS] Executando processo PID %d\n", getPid(p));
            printf("[FCFS] Executando processo PID %d\n", getPid(p));
            strcat(e->log_buffer, frase);
            e->final_buffer += sizeof(frase);
            ///////
        }
    }

    if(e->tam_buffer < e->final_buffer+70){
        e->log_buffer = realloc(e->log_buffer, (e->tam_buffer+TAM_BUFFER) * sizeof(char));
        e->tam_buffer += TAM_BUFFER;
    }

    snprintf(frase, sizeof(frase),  "Escalonador terminou execução de todos processos\n");
    printf("Escalonador terminou execução de todos processos\n");
    strcat(e->log_buffer, frase);
    e->final_buffer += sizeof(frase);
}

void imprimeBuffer(Escalonador *e, FILE *fp){
    fscanf(fp, "%s", e->log_buffer);
}

void adicionaProcesso(Escalonador *e, PCB *p){

    adicionaProcessoFila(e->pcb_list, p);
    adicionaProcessoFila(e->fila_prontos, p);
    e->qtd_processos++;
}

void liberaEscalonador(Escalonador *e){

    desalocaFilaProcessos(e->pcb_list);
    desalocaFilaProntos(e->fila_prontos);
    free(e->log_buffer);
    free(e);
}