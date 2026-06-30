/* ============================================================================
 *  memoria.h  -  Modulo 3: Gerenciamento de Memoria
 * ----------------------------------------------------------------------------
 *  Simula uma RAM de 1024 MB usando particionamento dinamico (variavel).
 *  A memoria e representada por uma lista de blocos contiguos, cada um
 *  livre ou ocupado por um processo.
 *
 *  Algoritmos de alocacao implementados:
 *    - First Fit : primeiro bloco livre suficiente
 *    - Best Fit  : menor bloco livre que atenda
 *    - Worst Fit : maior bloco livre disponivel
 *
 *  Conceitos demonstrados:
 *    - Fragmentacao externa: espaco livre total existe, mas espalhado em
 *      varios buracos pequenos (nenhum grande o suficiente).
 *    - Fragmentacao interna: requisicoes sao arredondadas para multiplos da
 *      unidade de alocacao; a sobra dentro do bloco e desperdicada.
 * ==========================================================================*/
#ifndef MEMORIA_H
#define MEMORIA_H

#define MEMORIA_TOTAL    1024  /* Tamanho total da RAM simulada (MB)        */
#define MAX_BLOCOS       256   /* Numero maximo de blocos na lista          */
#define UNIDADE_ALOCACAO 4     /* Granularidade da alocacao (gera frag. interna) */

/* Algoritmos de alocacao de memoria. */
typedef enum { FIRST_FIT, BEST_FIT, WORST_FIT } AlgoritmoFit;

/* Bloco contiguo de memoria. */
typedef struct {
    int inicio;       /* Endereco inicial do bloco (MB)            */
    int tamanho;      /* Tamanho do bloco (MB)                     */
    int livre;        /* 1 = livre, 0 = ocupado                    */
    int pid;          /* PID do dono (-1 se livre)                 */
    int requisitado;  /* Quanto o processo realmente pediu (MB)    */
} BlocoMemoria;

/* Inicializa a memoria como um unico bloco livre de 1024 MB. */
void inicializarMemoria(void);

/* Submenu de gerenciamento de memoria. */
void menuMemoria(void);

/* Operacoes principais. */
void alocarMemoria(void);     /* Aloca memoria para um processo (interativo) */
void liberarMemoria(void);    /* Libera a memoria de um processo             */
void exibirMapaMemoria(void); /* Mostra o mapa atual da RAM                  */

/* Uso da memoria, para o menu de Estatisticas (valores em MB). */
void obterUsoMemoria(int *total, int *usada, int *livre);

#endif /* MEMORIA_H */
