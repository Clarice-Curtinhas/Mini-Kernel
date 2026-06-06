#include "escalonador.h"

#define FCFS 1
#define RR 2
#define PP 3

#define TAM_BUFFER 250

#define TRUE 1
#define FALSE 0

typedef struct tEscalonador {
    int qtd_processos;
    double quantum;  //Quantum usado no Round Robin (ms) 
    char *log_buffer;  //Armazena  mensagens  de  log  durante  a execução
    int tam_buffer;
    int final_buffer;
    int total_prontos;
    Fila *pcb_list; //Lista de todos os processos do sistema
    Fila *fila_prontos; //Fila de prontos circular 
    int generator_done; //Sinaliza que todos os processos foram criados e enfileirados 
    PCB *current_process; //Processo atualmente em execução 
    int scheduler_type; //Define política de escalonamento 
    int tempo_atual;
    pthread_cond_t scheduler_cv;
    pthread_mutex_t scheduler_mutex;
} Escalonador;

Escalonador* criaEscalonador(int qtd_processos){

    Escalonador *e = malloc(sizeof(Escalonador));

    e->qtd_processos = qtd_processos;

    e->pcb_list = criaFila(qtd_processos);
    e->fila_prontos = criaFila(qtd_processos);

    pthread_cond_init(&e->scheduler_cv, NULL);
    pthread_mutex_init(&e->scheduler_mutex, NULL);

    e->log_buffer = malloc(TAM_BUFFER * sizeof(char)); 
    e->log_buffer[0] = '\0';
    e->tam_buffer = TAM_BUFFER;

    e->total_prontos = 0;

    e->quantum = 500;
    e->final_buffer = 0;
    e->current_process = NULL;
    e->scheduler_type = 0;
    e->tempo_atual = 0;
    e->generator_done = FALSE;

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

    qsort(getVetor(e->pcb_list), e->qtd_processos, sizeof(PCB*), comparaProcessos);
    
    if(e->scheduler_type == FCFS){
        escalonamentoFCFS(e);
    }

    else if(e->scheduler_type == RR){
        escalonamentoRR(e);
    }

    else{
        escalonamentoPP(e);
    }
}

void verificaProcessosValidos(Escalonador *e){

    int soma = 0;

    for(int i = 0; i < e->qtd_processos; i++){

        PCB *p = getProcesso(e->pcb_list, i);

        if(p == NULL) continue;

        if(getState(p) == FINISHED) continue;

        pthread_mutex_lock(&e->scheduler_mutex);

        if(getTempoChegada(p) <= e->tempo_atual){
        
            soma = adicionaProcessoFila(e->fila_prontos, p);
            e->total_prontos += soma;
        }

        pthread_mutex_unlock(&e->scheduler_mutex);
    }

    if(e->total_prontos == e->qtd_processos){
        e->generator_done = TRUE;
    }

    if(soma == 1) pthread_cond_broadcast(&e->scheduler_cv);
}

void escalonamentoFCFS(Escalonador *e){

    while(filaVazia(e->fila_prontos) == 0 || e->generator_done == FALSE){

        if(e->generator_done == FALSE) verificaProcessosValidos(e);
        
        if(e->current_process == NULL || getState(e->current_process) == FINISHED){
            
            if(e->current_process != NULL){
                finalizaPcbBuffer(e);
            }
            
            pthread_mutex_lock(&e->scheduler_mutex);
            
            PCB *p = retiraProcesso(e->fila_prontos);

            if(p == NULL){

                while(1){
                    
                    p = retiraProcesso(e->fila_prontos);
                    if(p != NULL) break;
                    pthread_cond_wait(&e->scheduler_cv, &e->scheduler_mutex);
                }
            }

            pthread_mutex_unlock(&e->scheduler_mutex);
        
            e->tempo_atual += getDuracao(p);

            e->current_process = p;

            pthread_mutex_t *mutex = getMutex(p);
            pthread_cond_t *cond = getCondicional(p);

            pthread_mutex_lock(mutex);
            setState(p, RUNNING);
            pthread_cond_broadcast(cond);

            while(getState(p) != FINISHED){
                pthread_cond_wait(cond, mutex);
            }
            
            pthread_mutex_unlock(mutex);

            executaPcbBuffer(e);
        }
    }

    finalizaPcbBuffer(e);
    terminaExecucaoBuffer(e);
}

void escalonamentoRR(Escalonador *e){
    
    //int time = 0;

    while(filaVazia(e->fila_prontos) == 0 || e->generator_done == FALSE){

        if(e->generator_done == FALSE) verificaProcessosValidos(e);
        
        //if(e->current_process == NULL || time != 500){
            
            pthread_mutex_lock(&e->scheduler_mutex);
            
            PCB *p = retiraProcesso(e->fila_prontos);

            if(p == NULL){

                while(1){
                    
                    p = retiraProcesso(e->fila_prontos);
                    if(p != NULL) break;
                    pthread_cond_wait(&e->scheduler_cv, &e->scheduler_mutex);
                }
            }

            pthread_mutex_unlock(&e->scheduler_mutex);
        
            e->tempo_atual += getDuracao(p);

            e->current_process = p;
            int falta = getRemainingTime(p) - e->quantum;

            pthread_mutex_t *mutex = getMutex(p);
            pthread_cond_t *cond = getCondicional(p);

            pthread_mutex_lock(mutex);
            setState(p, RUNNING);
            pthread_cond_broadcast(cond);

            while(getRemainingTime(p) > falta){
                pthread_cond_wait(cond, mutex);
            }

            if(getRemainingTime(p) > 0){
                setState(p, READY);
                adicionaProcessoFila(e->fila_prontos, p);
            }
            
            pthread_mutex_unlock(mutex);

            executaPcbBuffer(e);
        //}
    }

    finalizaPcbBuffer(e);
    terminaExecucaoBuffer(e);
}

void escalonamentoPP(Escalonador *e){
    return;
}

void realocaBuffer(Escalonador *e, int test){
    
    if(e->tam_buffer < e->final_buffer+test){
        e->log_buffer = realloc(e->log_buffer, (e->tam_buffer+TAM_BUFFER) * sizeof(char));
        e->tam_buffer += TAM_BUFFER;
    }
}

void terminaExecucaoBuffer(Escalonador *e){

    char frase[60];

    snprintf(frase, sizeof(frase),  "Escalonador terminou execução de todos processos\n");

    realocaBuffer(e, sizeof(frase));
    strcat(e->log_buffer, frase);
    e->final_buffer += sizeof(frase);
}

void finalizaPcbBuffer(Escalonador *e){
    
    char frase[60];
    
    if(e->scheduler_type == FCFS){
        snprintf(frase, sizeof(frase), "[FCFS] Processo PID %d finalizado\n", getPid(e->current_process));
        realocaBuffer(e, sizeof(frase));
        strcat(e->log_buffer, frase);
        e->final_buffer += sizeof(frase);
    }

    else if(e->scheduler_type == RR){
        snprintf(frase, sizeof(frase), "[RR] Processo PID %d finalizado\n", getPid(e->current_process));
        realocaBuffer(e, sizeof(frase));
        strcat(e->log_buffer, frase);
        e->final_buffer += sizeof(frase);
    }

    else if(e->scheduler_type == PP){
        snprintf(frase, sizeof(frase), "[PRIORITY] Processo PID %d finalizado\n", getPid(e->current_process));
        realocaBuffer(e, sizeof(frase));
        strcat(e->log_buffer, frase);
        e->final_buffer += sizeof(frase);
    }
}

void executaPcbBuffer(Escalonador *e){
    
    char frase[60];
    
    if(e->scheduler_type == FCFS){
        snprintf(frase, sizeof(frase), "[FCFS] Executando processo PID %d\n", getPid(e->current_process));
        realocaBuffer(e, sizeof(frase));
        strcat(e->log_buffer, frase);
        e->final_buffer += sizeof(frase);
    }

    else if(e->scheduler_type == RR){
        snprintf(frase, sizeof(frase), "[RR] Executando processo PID %d com quantum %.fms\n", getPid(e->current_process), e->quantum);
        realocaBuffer(e, sizeof(frase));
        strcat(e->log_buffer, frase);
        e->final_buffer += sizeof(frase);
    }

    else if(e->scheduler_type == PP){
        snprintf(frase, sizeof(frase), "[PRIORITY] Executando processo PID %d com prioridade %dms\n", getPid(e->current_process), getPrioridade(e->current_process));
        realocaBuffer(e, sizeof(frase));
        strcat(e->log_buffer, frase);
        e->final_buffer += sizeof(frase);
    }
}

void imprimeBuffer(Escalonador *e, FILE *fp){
    fprintf(fp, "%s", e->log_buffer);
}

void adicionaProcesso(Escalonador *e, PCB *p){

    adicionaProcessoFila(e->pcb_list, p);
}

void liberaEscalonador(Escalonador *e){

    pthread_cond_destroy(&e->scheduler_cv);
    pthread_mutex_destroy(&e->scheduler_mutex);
    
    desalocaFilaProcessos(e->pcb_list);
    desalocaFilaProntos(e->fila_prontos);
    free(e->log_buffer);
    free(e);
}