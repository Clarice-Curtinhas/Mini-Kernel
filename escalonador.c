#include "escalonador.h"

#define FCFS 1
#define RR 2
#define PP 3

#define TAM_BUFFER 250

#define TRUE 1
#define FALSE 0

#define MONO 0
#define MULTI 1

#define QTD_PROCESSADORES 2

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
    PCB *current_process_multi[QTD_PROCESSADORES];
    
    int scheduler_type; //Define política de escalonamento 
    int tempo_atual;
    int tipo_processador;
    int indice_processador;
    int terminou_buffer;
    int rodando; // processos que ainda estão rodando no multiprocessamento

    pthread_t *thread_id;
    pthread_cond_t scheduler_cv;
    pthread_mutex_t scheduler_mutex;
    pthread_mutex_t multi_mutex;
    pthread_mutex_t buffer_mutex;
} Escalonador;

Escalonador* criaEscalonador(int qtd_processos, int tipo_processador){

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
    e->scheduler_type = 0;
    e->tempo_atual = 0;
    e->thread_id = NULL;
    e->generator_done = FALSE;
    e->tipo_processador = tipo_processador;
    e->indice_processador = 0;
    e->terminou_buffer = 0;
    e->rodando = 0;

    if(tipo_processador == MONO){
        e->current_process = NULL;
    }
    else{
        pthread_mutex_init(&e->multi_mutex, NULL);
        pthread_mutex_init(&e->buffer_mutex, NULL);

        e->thread_id = malloc(QTD_PROCESSADORES * sizeof(pthread_t));
        for(int i = 0; i < QTD_PROCESSADORES; i++){
            e->current_process_multi[i] = NULL;
        }
    }

    return e;
}

void setPolitica(Escalonador *e, int escalonamento){
    e->scheduler_type = escalonamento;
}

Fila* getFila(Escalonador *e){
    return e->pcb_list;
}

pthread_t getThread(Escalonador *e, int i){
    return e->thread_id[i];
}

void *executaEscalonamento(void *arg){

    Escalonador *e = (Escalonador*)arg;

    qsort(getVetor(e->pcb_list), e->qtd_processos, sizeof(PCB*), comparaProcessos);

    if(e->tipo_processador == 1){
        pthread_mutex_lock(&e->multi_mutex);

        e->thread_id[e->indice_processador] = pthread_self();
        e->indice_processador++;
        
        pthread_mutex_unlock(&e->multi_mutex);
    }
    
    if(e->scheduler_type == FCFS){
        if(e->tipo_processador == 0){
            FCFS_mono(e);
        }
        else{
            FCFS_multi(e);
        }
    }

    else if(e->scheduler_type == RR){
        if(e->tipo_processador == 0){
            RR_mono(e);
        }
        else{
            RR_multi(e);
        }
    }

    else if(e->scheduler_type == PP){
        if(e->tipo_processador == 0){
            PP_mono(e);
        }
        else{
            PP_multi(e);
        }
    }

    return NULL;
}

void verificaProcessosValidos(Escalonador *e){

    int inseriu = 0;

    if(e->tipo_processador == MULTI){

        pthread_mutex_lock(&e->scheduler_mutex);
        
        for(int i = 0; i < e->qtd_processos; i++){
            
            pthread_mutex_lock(&e->multi_mutex);
            PCB *p = getProcesso(e->pcb_list, i);
            pthread_mutex_unlock(&e->multi_mutex);

            if(p == NULL) continue;

            if(getState(p) == FINISHED || getState(p) == RUNNING) continue;

            if(getTempoChegada(p) <= e->tempo_atual){
                if(verificaSeExiste(e->fila_prontos, p) != 1){
                    int buff = adicionaProcessoFila(e->fila_prontos, p);
                    e->total_prontos += 1;
                    inseriu = 1;
                }
            }
        }

        if(e->total_prontos == e->qtd_processos){
            e->generator_done = TRUE;
        }

        pthread_mutex_unlock(&e->scheduler_mutex);
    }

    else{

        for(int i = 0; i < e->qtd_processos; i++){

            PCB *p = getProcesso(e->pcb_list, i);

            if(p == NULL) continue;

            if(getState(p) == FINISHED || getState(p) == RUNNING) continue;

            pthread_mutex_lock(&e->scheduler_mutex);

            if(getTempoChegada(p) <= e->tempo_atual){

                inseriu = adicionaProcessoFila(e->fila_prontos, p);
                e->total_prontos += inseriu;
            }

            pthread_mutex_unlock(&e->scheduler_mutex);
        }

        printf("Total de processos prontos: %d e qntProcessos: %d\n", e->total_prontos, e->qtd_processos);

        if(e->total_prontos == e->qtd_processos){
            e->generator_done = TRUE;
        }
    }

    if(inseriu == 1) pthread_cond_broadcast(&e->scheduler_cv);
}

void FCFS_mono(Escalonador *e){

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
            setTipoEscalonamento(p, 1);
        
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

// Fiz outra funcao pq a outra estava mt doida
void FCFS_multi(Escalonador *e){

    int id;
    
    if(pthread_self() == e->thread_id[0]){
        id = 0;
    }
    else{
        id = 1;
    }

    while(filaVazia(e->fila_prontos) == 0 || e->generator_done == FALSE){

        if(e->generator_done == FALSE) verificaProcessosValidos(e);

        printf("Quantidade de processos prontos: %d\n", getTam(e->fila_prontos));
        
        if(e->current_process_multi[id] == NULL || getState(e->current_process_multi[id]) == FINISHED){
            
            pthread_mutex_lock(&e->scheduler_mutex);
            
            if(e->qtd_processos > 1){
                pthread_mutex_lock(&e->multi_mutex);
            }

            PCB *p = getPrimeiro(e->fila_prontos);

            if(p == NULL){

                while(1){
                    
                    p = getPrimeiro(e->fila_prontos);
                    if(p != NULL) break;
                    pthread_cond_wait(&e->scheduler_cv, &e->scheduler_mutex);
                }
            }

            e->tempo_atual += getDuracao(p);
            e->current_process_multi[id] = p;
            e->current_process = p;

            executaPcbBuffer(e);
            printf("Executando processo %d\n", getPid(p));
            
            if(e->qtd_processos > 1){
                pthread_mutex_unlock(&e->multi_mutex);
            }

            if(pthread_self() == e->thread_id[0]){
                printf("\n    Processador 0 pegou o processo %d\n", getPid(p));
            }
            else if(pthread_self() == e->thread_id[1]){
                printf("\n    Processador 1 pegou o processo %d\n", getPid(p));
            }

            pthread_mutex_unlock(&e->scheduler_mutex);
            setTipoEscalonamento(p, 1);

            pthread_mutex_t *mutex = getMutex(p);
            pthread_cond_t *cond = getCondicional(p);

            int falta = getRemainingTime(p) - e->quantum;

            pthread_mutex_lock(mutex);
            setState(p, RUNNING);
            setThreadsRestantes(p);
           
            pthread_cond_broadcast(cond);


            while(getRemainingTime(p) > falta && getState(p) != FINISHED){
                pthread_cond_wait(cond, mutex);
            }

            if(getThreadsRestantes(p) == 0 && getRemainingTime(p) <= 0){

                printf("Processo %d acabou!\n", getPid(p));

                pthread_mutex_lock(&e->scheduler_mutex);

                //Impressão de término do processo

                pthread_mutex_lock(&e->multi_mutex);
                printf("Processo %d saiu do lock!\n", getPid(p));

                e->current_process = p;
                finalizaPcbBuffer(e);
                RetiraProcessoEspecifico(e->fila_prontos, p);

                pthread_mutex_unlock(&e->multi_mutex);

                ///acaba

                e->current_process_multi[id] = NULL;

                pthread_mutex_unlock(&e->scheduler_mutex);
                printf("Processo %d acabou!\n", getPid(p));
            }
            
            pthread_mutex_unlock(mutex);
        }
    }
}

void RR_mono(Escalonador *e){
    
    while(filaVazia(e->fila_prontos) == 0 || e->generator_done == FALSE){

        if(e->generator_done == FALSE) verificaProcessosValidos(e);
                    
        pthread_mutex_lock(&e->scheduler_mutex);
        PCB *p = retiraProcesso(e->fila_prontos);

        if(p == NULL){

            while(1){
                    
                p = retiraProcesso(e->fila_prontos);
                if(p != NULL) break;
                pthread_cond_wait(&e->scheduler_cv, &e->scheduler_mutex);
            }
        }

        printf("Processo %d retirado\n", getPid(p));

        setTipoEscalonamento(p, 2);

        pthread_mutex_unlock(&e->scheduler_mutex);
        
        e->tempo_atual += e->quantum;
        
        e->current_process = p;
        int falta = getRemainingTime(p) - e->quantum;
        if(falta < 0) falta = 0;
        printf("1-Get remaining time: %d e falta: %d\n", getRemainingTime(p), falta);

        pthread_mutex_t *mutex = getMutex(p);
        pthread_cond_t *cond = getCondicional(p);

        pthread_mutex_lock(mutex);
        setState(p, RUNNING);
        printf("Processo %d running, Remaining time: %d\n", getPid(p), getRemainingTime(p));
        pthread_cond_broadcast(cond);

        while(getRemainingTime(p) > falta && getState(p) != FINISHED){
            pthread_cond_wait(cond, mutex);
        }
        printf("2-Get remaining time: %d e falta: %d\n", getRemainingTime(p), falta);
            
        pthread_mutex_unlock(mutex);

        if(getRemainingTime(p) > 0){
            adicionaProcessoFila(e->fila_prontos, p);
        }

        executaPcbBuffer(e);

        if(getRemainingTime(p) <= 0){
            printf("Processo %d acabou\n", getPid(p));
            finalizaPcbBuffer(e);
        }
    }

    terminaExecucaoBuffer(e);
}

void RR_multi(Escalonador *e){

    while(filaVazia(e->fila_prontos) == 0 || e->generator_done == FALSE){

        if(filaVazia(e->fila_prontos) == 1 && e->generator_done == TRUE) break;

        if(e->generator_done == FALSE) verificaProcessosValidos(e);
                    
        pthread_mutex_lock(&e->scheduler_mutex);

        if(e->qtd_processos > 1){
            pthread_mutex_lock(&e->multi_mutex);
        }

        int primeiro = 0;
        PCB *p = getPrimeiro(e->fila_prontos);

        if(p == NULL){

            while(1){
                    
                p = getPrimeiro(e->fila_prontos);
                if(p != NULL) break;
                pthread_cond_wait(&e->scheduler_cv, &e->scheduler_mutex);
            }
        }

        printf("Pegando processo %d\n", getPid(p));
        e->current_process = p;
        executaPcbBuffer(e);
        e->tempo_atual += e->quantum;

        if(e->qtd_processos > 1){
            pthread_mutex_unlock(&e->multi_mutex);
        }
        
        if(pthread_self() == e->thread_id[0]){
            printf("Processador 0 pegou o processo %d\n", getPid(p));
        }

        else if(pthread_self() == e->thread_id[1]){
            printf("Processador 1 pegou o processo %d\n", getPid(p));
        }
        setTipoEscalonamento(p, 2);

        pthread_mutex_unlock(&e->scheduler_mutex);

        pthread_mutex_t *mutex = getMutex(p);
        pthread_cond_t *cond = getCondicional(p);

        pthread_mutex_lock(mutex);

        int falta = getRemainingTime(p) - e->quantum;
        if(falta < 0) falta = 0;

        setState(p, RUNNING);
        setThreadsRestantes(p);
        printf("Processo %d running, Remaining time: %d\n", getPid(p), getRemainingTime(p));

        pthread_cond_broadcast(cond);

        while(getRemainingTime(p) > falta){
            pthread_cond_wait(cond, mutex);
        }

        e->current_process = p;

        if(pthread_self() == e->thread_id[0]){
            printf("Processador 0 acabou de executar o processo %d\n", getPid(p));
        }

        else if(pthread_self() == e->thread_id[1]){
            printf("Processador 1 acabou de executar o processo %d\n", getPid(p));
        }

        if(getRemainingTime(p) <= 0){
            printf("Processo %d acabou\n", getPid(p));

            pthread_mutex_lock(&e->multi_mutex);
                            
            finalizaPcbBuffer(e);

            pthread_mutex_unlock(&e->multi_mutex);
        }
        
        if(getRemainingTime(p) > 0){

            pthread_mutex_lock(&e->scheduler_mutex);

            retiraProcesso(e->fila_prontos);
            adicionaProcessoFila(e->fila_prontos, p);
            printf("Processo %d saiu e voltou p fila...\n", getPid(p));

            pthread_mutex_unlock(&e->scheduler_mutex);
        }
        else{

            printf("Processo %d acabou, retirando da fila\n", getPid(p));
            pthread_mutex_lock(&e->scheduler_mutex);
            retiraProcesso(e->fila_prontos);
            pthread_mutex_unlock(&e->scheduler_mutex);
        }

        pthread_mutex_unlock(mutex);
    }
}

void PP_mono(Escalonador *e){

    PCB *antigo, *atual;
    antigo = atual = NULL;
    
    while(filaVazia(e->fila_prontos) == 0 || e->generator_done == FALSE){
        printf("Voltou? Fila: %d e generator_done: %d\n", getTam(e->fila_prontos), e->generator_done);

        if(e->generator_done == FALSE) verificaProcessosValidos(e);
                    
        pthread_mutex_lock(&e->scheduler_mutex);
            
        PCB *p = getProcessoMaiorPrioridade(e->fila_prontos);
        atual = p;

        if(p == NULL){

            while(1){
                    
                p = getProcessoMaiorPrioridade(e->fila_prontos);
                atual = p;
                if(p != NULL) break;
                pthread_cond_wait(&e->scheduler_cv, &e->scheduler_mutex);
            }
        }

        setTipoEscalonamento(p, 3);

        pthread_mutex_unlock(&e->scheduler_mutex);
        
        e->tempo_atual += e->quantum;

        e->current_process = p;
        int falta = getRemainingTime(p) - e->quantum;
        if(falta < 0) falta = 0;

        pthread_mutex_t *mutex = getMutex(p);
        pthread_cond_t *cond = getCondicional(p);

        pthread_mutex_lock(mutex);
        setState(p, RUNNING);
        //printf("Processo %d running, Remaining time: %d\n", getPid(p), getRemainingTime(p));
        pthread_cond_broadcast(cond);

        while(getRemainingTime(p) > falta){
            pthread_cond_wait(cond, mutex);
            printf("remaining time: %d e falta: %d\n", getRemainingTime(p), falta);
        }

        printf("saiu do tcb\n");
            
        pthread_mutex_unlock(mutex);

        //printf("Processo %d faltando %d\n", getPid(p), getRemainingTime(p));

        if(antigo != atual){
            executaPcbBuffer(e);
        }
        
        if(getRemainingTime(p) > 0){
            antigo = p;
            adicionaProcessoFila(e->fila_prontos, p);
        }

        if(getRemainingTime(p) <= 0){
            printf("Processo %d acabou\n", getPid(p));
            finalizaPcbBuffer(e);
        }
    }

    terminaExecucaoBuffer(e);
}

void PP_multi(Escalonador *e){

    printf("\nEntrou no PP multi\n");

    PCB *antigo, *atual;
    antigo = atual = NULL;
    
    while(filaVazia(e->fila_prontos) == 0 || e->generator_done == FALSE){

        if(e->generator_done == FALSE) verificaProcessosValidos(e);
        
        printf("Tempo atual: %d\n", e->tempo_atual);
                    
        pthread_mutex_lock(&e->scheduler_mutex);

        if(e->qtd_processos > 1){
            pthread_mutex_lock(&e->multi_mutex);
        }
            
        PCB *p = getMaiorPrioridadeMulti(e->fila_prontos);
        atual = p;

        if(p == NULL){

            while(1){
                    
                p = getMaiorPrioridadeMulti(e->fila_prontos);
                atual = p;
                if(p != NULL) break;
                pthread_cond_wait(&e->scheduler_cv, &e->scheduler_mutex);
            }
        }
        
        setTipoEscalonamento(p, 3);
        e->current_process = p;
        executaPcbBuffer(e);

        if(e->qtd_processos > 1){
            pthread_mutex_unlock(&e->multi_mutex);
        }

        if(pthread_self() == e->thread_id[0]){
            printf("Processador 0 pegou o processo %d\n", getPid(p));
        }

        else if(pthread_self() == e->thread_id[1]){
            printf("Processador 1 pegou o processo %d\n", getPid(p));
        }

        pthread_mutex_unlock(&e->scheduler_mutex);

        pthread_mutex_t *mutex = getMutex(p);
        pthread_cond_t *cond = getCondicional(p);

        pthread_mutex_lock(mutex);

        pthread_mutex_lock(&e->multi_mutex);
        int falta = getRemainingTime(p) - e->quantum;
        if(falta < 0) falta = 0;

        setState(p, RUNNING);
        setThreadsRestantes(p);
        pthread_mutex_unlock(&e->multi_mutex);

        //printf("Processo %d running, Remaining time: %d\n", getPid(p), getRemainingTime(p));
        pthread_cond_broadcast(cond);

        while(getRemainingTime(p) > falta){
            pthread_cond_wait(cond, mutex);
        }

        pthread_mutex_unlock(mutex);
        //printf("Processo %d faltando %d\n", getPid(p), getRemainingTime(p));

        pthread_mutex_lock(&e->multi_mutex);
        e->tempo_atual += e->quantum;
        printf("tempo atual incrementado: %d\n", e->tempo_atual);

        
        if(getRemainingTime(p) > 0){
            pthread_mutex_lock(&e->scheduler_mutex);
            antigo = p;
            pthread_mutex_unlock(&e->scheduler_mutex);
        }

        pthread_mutex_unlock(&e->multi_mutex);

        if(getRemainingTime(p) <= 0 || getState(p) == FINISHED){
            printf("    Processo %d imprimiu término\n", getPid(p));

            //Impressão de término do processo

            pthread_mutex_lock(&e->multi_mutex);
            e->current_process = atual;
            RetiraProcessoEspecifico(e->fila_prontos, atual);
            finalizaPcbBuffer(e);

            pthread_mutex_unlock(&e->multi_mutex);

            ///acaba
        }
    }
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

    if(getFinalizado(e->current_process) == 1) return;
    
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

    setFinalizado(e->current_process);
}

void executaPcbBuffer(Escalonador *e){
    
    char frase[75];
    
    if(e->scheduler_type == FCFS){
        snprintf(frase, sizeof(frase), "[FCFS] Executando processo PID %d", getPid(e->current_process));
        realocaBuffer(e, sizeof(frase));
        strcat(e->log_buffer, frase);
        e->final_buffer += sizeof(frase);
    }

    else if(e->scheduler_type == RR){
        snprintf(frase, sizeof(frase), "[RR] Executando processo PID %d com quantum %.fms", getPid(e->current_process), e->quantum);
        realocaBuffer(e, sizeof(frase));
        strcat(e->log_buffer, frase);
        e->final_buffer += sizeof(frase);
    }

    else if(e->scheduler_type == PP){
        snprintf(frase, sizeof(frase), "[PRIORITY] Executando processo PID %d prioridade %d", getPid(e->current_process), getPrioridade(e->current_process));
        realocaBuffer(e, sizeof(frase));
        strcat(e->log_buffer, frase);
        e->final_buffer += sizeof(frase);
    }

    if(e->tipo_processador == 0){
        snprintf(frase, sizeof(frase), "\n");
        realocaBuffer(e, sizeof(frase));
        strcat(e->log_buffer, frase);
        e->final_buffer += sizeof(frase);
    }
    else{
        if(pthread_self() == e->thread_id[0]){
            snprintf(frase, sizeof(frase), " // processador 0\n");
        }
        else{
            snprintf(frase, sizeof(frase), " // processador 1\n");
        }
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

    if(e->tipo_processador == MULTI){
        pthread_mutex_destroy(&e->multi_mutex);
        pthread_mutex_destroy(&e->buffer_mutex);
        free(e->thread_id);
    }
    
    desalocaFilaProcessos(e->pcb_list);
    desalocaFilaProntos(e->fila_prontos);
    free(e->log_buffer);
    free(e);
}