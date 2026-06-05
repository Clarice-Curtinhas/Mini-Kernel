#ifndef TCB_H
#define TCB_H

#include <pthread.h>
#include "PCB.h"

typedef struct tTCB TCB;

void *thr_func(void *arg);

#endif