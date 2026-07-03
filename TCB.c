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

    pthread_mutex_t *mutex = getMutex(p);
    
    while(getState(p) != FINISHED){
        
        pthread_mutex_lock(mutex);
        
        while(getState(p) != RUNNING){

            if(getState(p) == FINISHED) break;
            pthread_cond_t *cond = getCondicional(p);
            pthread_cond_wait(cond, mutex);
        }
         
        int tipo_escalonador = getTipoEscalonamento(p);

        if((tipo_escalonador == 2  || tipo_escalonador == 3) && tempo >= 500){
            printf("[TCB] Processo %d Remaining time %d\n", getPid(p), getRemainingTime(p));
            usleep(500 * 1000);
            setState(p, READY);
            diminuiRemainingTime(p, 500);
        }

        else{
            usleep(tempo * 1000);
            diminuiRemainingTime(p, tempo);
        }

        pthread_mutex_unlock(mutex);
    }
    //printf("Terminou thread processo %d\n", getPid(p));
}
 
