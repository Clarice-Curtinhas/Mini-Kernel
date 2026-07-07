#ifndef TCB_H
#define TCB_H

#include <pthread.h>
#include "PCB.h"

typedef struct tTCB TCB;

/**
 * @brief Simula a execução de uma thread e define seu tratamento, a depender do tipo de escalonamento e de processador.
 * @param void O processo responsável por aquela thread.
 */
void *thr_func(void *arg);

#endif