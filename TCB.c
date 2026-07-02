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

            pthread_cond_t *cond = getCondicional(p);
            pthread_cond_wait(cond, mutex);
        }

        pthread_mutex_unlock(mutex);

        if(getTipoEscalonamento(p) == 2 && tempo > 500){
            usleep(500 * 1000);
            pthread_mutex_lock(mutex);
            setState(p, READY);
            diminuiRemainingTime(p, 500);
            pthread_mutex_unlock(mutex);
        }
        else{
            usleep(tempo * 1000);
            pthread_mutex_lock(mutex);
            diminuiRemainingTime(p, tempo);
            pthread_mutex_unlock(mutex);
        }
    }
}
 
