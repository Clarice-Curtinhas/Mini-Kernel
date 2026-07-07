#ifndef PCB_H
#define PCB_H

#include <stdio.h>
#include <stdlib.h>
#include "TCB.h"

#define READY 'p'
#define RUNNING 'r'
#define FINISHED 'f'

typedef char ProcessState;

typedef struct tPCB PCB;

/**
 * @brief Aloca e inicializa um novo processo.
 * @param int Duração do processo. 
 * @param int prioridade do processo.
 * @param int qtd de threads do processo.
 * @param int instante em que o processo chegou no processador.
 * @param int pid do processo.
 * @param int tipo de processador que vai executar o processo (mono ou multi).
 * @return O processo.
 */
PCB* criaProcesso(int duracao, int prioridade, int n_threads, int tempo_chegada, int pid, int tipo_processador);

/**
 * @brief Obtém o estado do processo (READY, RUNNING OU FINISHED).
 * @param PCB O processo.
 * @return O estado do processo.
 */
ProcessState getState(PCB *processo);

/**
 * @brief Define um novo estado para o processo (READY, RUNNING OU FINISHED).
 * @param PCB O processo.
 * @param ProcessState O novo estado do processo.
 */
void setState(PCB *processo, ProcessState novoEstado);

/**
 * @brief Reseta a quantidade de threads que um processo já executou.
 * @param PCB O processo.
 */
void resetThreadsQuantum(PCB *processo);

/**
 * @brief Incrementa a quantidade de threads que um processo já executou.
 * @param PCB O processo.
 */
void setThreadsQuantum(PCB *processo);

/**
 * @brief Obtém a quantidade de threads que um processo já executou.
 * @param PCB O processo.
 * @return A quantidade de threads que um processo já executou.
 */
int getThreadsQuantum(PCB *processo);

/**
 * @brief Obtém o PID do processo.
 * @param PCB O processo.
 * @return O PID do processo.
 */
int getPid(PCB *processo);

/**
 * @brief Obtém a duração de um processo.
 * @param PCB O processo.
 * @return A duração de um processo.
 */
int getDuracao(PCB *processo);

/**
 * @brief Obtém o quantum de um processo.
 * @param PCB O processo.
 * @return O quantum de um processo.
 */
int getQuantumProcesso(PCB *processo);

/**
 * @brief Obtém a qtd de threads que o processo precisa executar para terminar.
 * @param PCB O processo.
 * @return A qtd de threads restantes.
 */
int getThreadsRestantes(PCB *processo);

/**
 * @brief Define o processo atual como finalizado.
 * @param PCB O processo.
 */
void setFinalizado(PCB *processo);

/**
 * @brief Verifica se um processo já está finalizado.
 * @param PCB O processo.
 * @return 1 se o processo já foi finalizado.
 */
int getFinalizado(PCB *processo);

/**
 * @brief Decremeta a qtd de threads que faltam para um processo acabar.
 * @param PCB O processo.
 */
void setThreadsRestantes(PCB *processo);

/**
 * @brief Obtém o tipo de processador (MONO ou MULTI) que está executando o processo.
 * @param PCB O processo.
 * @return 0 se for MONO e 1 se for MULTI.
 */
int getTipoProcessador(PCB *processo);

/**
 * @brief Obtém o tempo que falta para um processo finalizar.
 * @param PCB O processo.
 * @return O tempo restante de um processo.
 */
int getRemainingTime(PCB *processo);

/**
 * @brief Obtém a qtd de threads de um processo.
 * @param PCB O processo.
 * @return A qtd de threads de um processo.
 */
int getNumThreads(PCB *processo);

/**
 * @brief Obtém o tempo de chegada de um processo.
 * @param PCB O processo.
 * @return O tempo de chegada de um processo.
 */
int getTempoChegada(PCB *processo);

/**
 * @brief Obtém a prioridade de um processo.
 * @param PCB O processo.
 * @return A prioridade de um processo.
 */
int getPrioridade(PCB *processo);

/**
 * @brief Obtém o tipo de escalonamento (FCFS, RR ou PP) de um processo.
 * @param PCB O processo.
 * @return O tipo de escalonamento de um processo.
 */
int getTipoEscalonamento(PCB *processo);

/**
 * @brief Define o tipo de escalonamento (FCFS, RR ou PP) de um processo.
 * @param PCB O processo.
 */
void setTipoEscalonamento(PCB *processo, int tipo);

/**
 * @brief Obtém o tipo de escalonamento (FCFS, RR ou PP) de um processo.
 * @param PCB O processo.
 * @return O tipo de escalonamento de um processo.
 */
pthread_mutex_t* getMutex(PCB *processo);

/**
 * @brief Compara dois processos pelo tempo de chegada.
 * @param void Processo 1.
 * @param void Processo 2.
 * @return 1 se o tempo de chegada do primeiro processo for menor que o do segundo.
 */
int comparaProcessos(const void *a, const void *b);

/**
 * @brief Obtém a variável condicional do processo.
 * @param PCB O processo.
 * @return A variável condicional do processo.
 */
pthread_cond_t* getCondicional(PCB *processo);

/**
 * @brief Diminui o tempo restante de um processo. Se esse tempo chegar a 0, muda o estado do processo para FINISHED.
 * @param PCB O processo.
 * @param PCB O quantum que ele executou.
 */
void diminuiRemainingTime(PCB *processo, int valor);

/**
 * @brief Imprime um processo e seus atributos.
 * @param PCB O processo.
 * @param FILE O arquivo de impressão.
 */
void imprimeProcesso(FILE *fp, PCB *processo);

/**
 * @brief Libera toda a memória usada por um processo, destruindo os mutexes e dando um join nas suas threads.
 * @param PCB O processo.
 */
void desalocaProcesso(PCB *processo);

#endif