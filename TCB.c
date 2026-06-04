#include "TCB.h"

typedef struct Tcb {
   PCB* processo;
   int thread_index;
} tTcb;

tTcb* criaThread(PCB *processo){

   tTcb *thread = malloc(sizeof(Tcb));

   thread->processo = processo;
   pthread_create(&thread->thread_index, NULL, thr_func, NULL);

   return thread;
}

void *thr_func(void *arg){
    
}
