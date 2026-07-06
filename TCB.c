#include <unistd.h>
#include "TCB.h"

#define READY 'p'
#define RUNNING 'r'
#define FINISHED 'f'

typedef struct tTCB {
    PCB* processo;
    int thread_index;
} TCB;

void *thr_func(void *arg){
   
    PCB *p = (PCB*)arg;
    int tempo = getDuracao(p)/getNumThreads(p);
    int tipo_processador = getTipoProcessador(p);

    pthread_mutex_t *mutex = getMutex(p);
    pthread_mutex_lock(mutex);
    
    while(getState(p) != FINISHED){
        
        while(getState(p) != RUNNING){

            if(getState(p) == FINISHED) break;
            pthread_cond_t *cond = getCondicional(p);
            pthread_cond_wait(cond, mutex);
        }
         
        int tipo_escalonador = getTipoEscalonamento(p);

        if(tipo_processador == 0){

            if((tipo_escalonador == 2  || tipo_escalonador == 3) && tempo >= 500){
                printf("[TCB] Processo %d Remaining time %d\n", getPid(p), getRemainingTime(p));
                usleep(500 * 1000);
                setState(p, READY);
                diminuiRemainingTime(p, 500);
            }

            else{
                usleep(tempo * 1000);
                printf("TCB vai diminuir %d do processo %d\n", tempo, getPid(p));
                diminuiRemainingTime(p, tempo);
            }
        }
        
        else{
            if(tipo_escalonador == 1){
                printf("[TCB] Processo %d executando, Remaining time %d\n", getPid(p), getRemainingTime(p));
                usleep(500 * 1000);
                //setState(p, READY);
                diminuiRemainingTime(p, 500);
            }

            else if(tipo_escalonador == 2 || tipo_escalonador == 3){
                printf("[TCB] Processo %d Remaining time %d\n", getPid(p), getRemainingTime(p));
                usleep(500 * 1000);
                setState(p, READY);
                diminuiRemainingTime(p, 500);
            }
        }
    }

    pthread_mutex_unlock(mutex);
    //printf("Terminou thread processo %d\n", getPid(p));
    
    return NULL;
}
 
