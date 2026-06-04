#ifndef TCB_H
#define TCB_H

#include <pthreads.h>

typedef struct Tcb tTcb;

tTcb* criaThread(PCB *processo);

void *thr_func(void *arg);

#endif