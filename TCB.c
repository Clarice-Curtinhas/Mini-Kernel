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
    
    pthread_mutex_lock(mutex);
   
    while(getState(p) != RUNNING){

        //printf("Bloqueado!\n");

        pthread_cond_t *cond = getCondicional(p);
        pthread_cond_wait(cond, mutex);
    }

    //printf("Desbloqueado! Processo %d\n",getPid(p));

    usleep(tempo*1000);
    diminuiRemainingTime(p, tempo);

    pthread_mutex_unlock(mutex);

    return NULL;
}
 
