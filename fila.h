#ifndef FILA_H
#define FILA_H

#include <stdlib.h>
#include "PCB.h"

typedef struct tFila Fila;

PCB* getPrimeiroRR(Fila *fila);

/**
 * @brief Aloca e inicializa uma nova fila circular.
 * @param int Qtd de processos com que a fila será criada. 
 * @return A fila.
 */
Fila *criaFila(int num_processos);

/**
 * @brief Adiciona um processo no final da fila.
 * @param Fila A fila.
 * @param PCB O processo que será adicionado.
 * @return 1 se ele foi adicionado.
 */
int adicionaProcessoFila(Fila *fila, PCB *p);

/**
 * @brief Retira o primeiro processo da fila.
 * @param Fila A fila.
 * @return O processo retirado.
 */
PCB *retiraProcesso(Fila *fila);

/**
 * @brief Verifica se a fila está vazia (sem nenhum processo).
 * @param Fila A fila.
 * @return 1 se a fila estiver vazia.
 */
int filaVazia(Fila *fila);

/**
 * @brief Verifica se um processo está na fila.
 * @param Fila A fila.
 * @param PCB O processo que está sendo verificado.
 * @return 1 se o processo existir na fila.
 */
int verificaSeExiste(Fila *fila, PCB *p);

/**
 * @brief Obtém o vetor de processos da fila.
 * @param Fila A fila.
 * @return O vetor de processos da fila.
 */
PCB** getVetor(Fila *fila);

/**
 * @brief Obtém o processo de maior prioridade, retirando ele da fila.
 * @param Fila A fila.
 * @return O processo de maior prioridade da fila.
 */
PCB* getProcessoMaiorPrioridade(Fila *fila);

/**
 * @brief Obtém o processo de maior prioridade da fila, mas sem retirar.
 * @param Fila A fila.
 * @return O processo de maior prioridade da fila.
 */
PCB* getMaiorPrioridadeMulti(Fila *fila);

/**
 * @brief Obtém o tamanho atual da fila.
 * @param Fila A fila.
 * @return O tamanho atual da fila.
 */
int getTam(Fila *fila);

/**
 * @brief Obtém o primeiro processo da fila.
 * @param Fila A fila.
 * @return O primeiro processo da fila.
 */
PCB* getPrimeiro(Fila *fila);

/**
 * @brief Obtém o processo de uma posição específica da fila.
 * @param Fila A fila.
 * @param Fila A posição do processo buscado.
 * @return O processo.
 */
PCB* getProcesso(Fila *fila, int i);

/**
 * @brief Retira um processo específico da fila.
 * @param Fila A fila.
 * @param PCB O processo a ser retirado.
 */
void RetiraProcessoEspecifico(Fila *fila, PCB *p);

/**
 * @brief Libera toda a memória usada por uma fila, incluindo seus processos.
 * @param Fila A fila.
 */
void desalocaFilaProcessos(Fila *fila);

/**
 * @brief Libera toda a memória usada por uma fila.
 * @param Fila A fila.
 */
void desalocaFilaProntos(Fila *fila);

/**
 * @brief Imprime a fila e seus processos.
 * @param Fila A fila.
 * @param FILE O arquivo de impressão.
 */
void imprimeFila(FILE *fp, Fila *fila);

#endif