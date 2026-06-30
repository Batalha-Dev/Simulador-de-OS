/* ============================================================================
 *  escalonador.h  -  Modulo 2: Escalonamento da CPU
 * ----------------------------------------------------------------------------
 *  Implementa tres algoritmos de escalonamento:
 *    - FCFS (First Come First Served)
 *    - Round Robin (com quantum configuravel)
 *    - Prioridade (nao preemptivo)
 *
 *  Apos cada execucao sao calculadas as metricas classicas:
 *    - Tempo medio de espera
 *    - Tempo medio de retorno (turnaround)
 *    - Throughput (processos por unidade de tempo)
 *
 *  Premissa didatica: considera-se que todos os processos PRONTOS chegam
 *  no instante 0 (modelo classico de livro-texto), e a ordem de chegada
 *  e dada pela ordem de criacao dos processos.
 * ==========================================================================*/
#ifndef ESCALONADOR_H
#define ESCALONADOR_H

/* Estatisticas da ultima execucao do escalonador. */
typedef struct {
    char   algoritmo[40];     /* Nome do algoritmo usado            */
    int    numProcessos;      /* Quantidade de processos escalonados*/
    double tempoMedioEspera;  /* Media dos tempos de espera         */
    double tempoMedioRetorno; /* Media dos tempos de retorno        */
    double throughput;        /* Processos / tempo total            */
    int    tempoTotal;        /* Tempo total de simulacao           */
    int    valida;            /* 1 se ja houve uma execucao         */
} Estatisticas;

/* Submenu do escalonador (escolha do algoritmo). */
void menuEscalonador(void);

/* Algoritmos individuais (tambem chamaveis diretamente). */
void escalonarFCFS(void);
void escalonarRoundRobin(void);
void escalonarPrioridade(void);

/* Retorna as estatisticas da ultima execucao (para o menu de Estatisticas). */
Estatisticas obterEstatisticas(void);

#endif /* ESCALONADOR_H */
