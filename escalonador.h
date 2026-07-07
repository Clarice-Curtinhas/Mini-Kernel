#ifndef ESCALONADOR_H
#define ESCALONADOR_H

#include "PCB.h"
#include "fila.h"

#include <stdio.h>
#include <string.h>
#include <pthread.h>

typedef struct tEscalonador Escalonador;

/**
 * @brief Aloca e inicializa um escalonador, criando as filas dos processos que serão escalonados e os mutexes que serão utilizados.
 * @param int Qtd de processos que serão escalonados.
 * @param int Tipo de processador (MONO ou MULTI) que o escalonador vai tratar. 
 * @return O escalonador.
 */
Escalonador* criaEscalonador(int qtd_processos, int tipo_processador);

/**
 * @brief Obtém a lista de todos os processos do sistema.
 * @param Escalonador O escalonador.
 * @return A fila com todos os processos do sistema.
 */
Fila* getFila(Escalonador *e);

/**
 * @brief Obtém uma das threads que estão executando.
 * @param Escalonador O escalonador.
 * @param int O id da thread.
 * @return A thread.
 */
pthread_t getThread(Escalonador *e, int i);

/**
 * @brief Define a política de escalonamento (FCFS, RR ou PP).
 * @param Escalonador O escalonador.
 * @param int O tipo de escalonamento.
 */
void setPolitica(Escalonador *e, int escalonamento);

/**
 * @brief Chama as funções de escalonamento com base no tipo de escalonamento e processador.
 * @param void O escalonador.
 */
void *executaEscalonamento(void *arg);

/**
 * @brief Executa o escalonamento FCFS para monoprocessador.
 * @param Escalonador O escalonador.
 */
void FCFS_mono(Escalonador *e);

/**
 * @brief Executa o escalonamento FCFS para multiprocessador.
 * @param Escalonador O escalonador.
 */
void FCFS_multi(Escalonador *e);

/**
 * @brief Executa o escalonamento Round Robin para monoprocessador.
 * @param Escalonador O escalonador.
 */
void RR_mono(Escalonador *e);

/**
 * @brief Executa o escalonamento Round Robin para multiprocessador.
 * @param Escalonador O escalonador.
 */
void RR_multi(Escalonador *e);

/**
 * @brief Executa o escalonamento por Prioridade (com preempção) para monoprocessador.
 * @param Escalonador O escalonador.
 */
void PP_mono(Escalonador *e);

/**
 * @brief Executa o escalonamento por Prioridade (com preempção) para multiprocessador.
 * @param Escalonador O escalonador.
 */
void PP_multi(Escalonador *e);

/**
 * @brief Realoca o tamanho do buffer de impressão.
 * @param Escalonador O escalonador.
 * @param int Tamanho do texto a ser adicionado no buffer.
 */
void realocaBuffer(Escalonador *e, int text);

/**
 * @brief Adiciona os processos na fila de prontos conforme o tempo atual chega no tempo de chegada deles.
 * @param Escalonador O escalonador.
 */
void verificaProcessosValidos(Escalonador *e);

/**
 * @brief Imprime a mensagem de fim de escalonamento.
 * @param Escalonador O escalonador.
 */
void terminaExecucaoBuffer(Escalonador *e);

/**
 * @brief Imprime a mensagem de fim de execução de um processo.
 * @param Escalonador O escalonador.
 */
void finalizaPcbBuffer(Escalonador *e);

/**
 * @brief Imprime a mensagem de execução de um processo.
 * @param Escalonador O escalonador.
 */
void executaPcbBuffer(Escalonador *e);

/**
 * @brief Adiciona um processo na fila com todos os processos do sistema.
 * @param Escalonador O escalonador.
 * @param PCB O processo.
 */
void adicionaProcesso(Escalonador *e, PCB *p);

/**
 * @brief Imprime o buffer de mensagens.
 * @param Escalonador O escalonador.
 * @param FILE O arquivo de impressão.
 */
void imprimeBuffer(Escalonador *e, FILE *fp);

/**
 * @brief Libera toda a memória usada pelo escalonador, destruindo os mutexes e desalocando suas filas.
 * @param Escalonador O escalonador.
 */
void liberaEscalonador(Escalonador *e);

#endif