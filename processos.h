/* ============================================================================
 *  processos.h  -  Modulo 1: Gerenciamento de Processos
 * ----------------------------------------------------------------------------
 *  Define a estrutura Processo, os estados possiveis e as operacoes de
 *  criacao, encerramento, alteracao de estado e listagem.
 *
 *  A tabela de processos vive neste modulo e e compartilhada com os demais
 *  (escalonador, memoria, dispositivos) por meio das funcoes de acesso.
 * ==========================================================================*/
#ifndef PROCESSOS_H
#define PROCESSOS_H

#define MAX_PROCESSOS 100   /* Capacidade maxima da tabela de processos */

/* Estados de um processo, conforme o ciclo de vida classico de um SO. */
typedef enum {
    NOVO = 0,       /* Recem-criado (NEW)            */
    PRONTO,         /* Pronto para executar (READY)  */
    EXECUTANDO,     /* Em execucao na CPU (RUNNING)  */
    BLOQUEADO,      /* Aguardando recurso (WAITING)  */
    FINALIZADO      /* Encerrado (TERMINATED)        */
} Estado;

/* Estrutura que representa um processo (PCB simplificado). */
typedef struct {
    int   pid;            /* Identificador unico do processo            */
    char  nome[50];       /* Nome do processo (ex.: "Chrome")           */
    int   prioridade;     /* Prioridade (menor valor = maior prioridade)*/
    int   tempoExecucao;  /* Tempo total de CPU necessario (burst)      */
    Estado estado;        /* Estado atual                               */
    int   tempoChegada;   /* Ordem/instante de chegada (para metricas)  */
} Processo;

/* ---- Operacoes interativas (chamadas pelo menu) ---- */
void menuProcessos(void);     /* Submenu: criar / encerrar / alterar estado */
void criarProcesso(void);     /* Cria um processo a partir da entrada do usuario */
void encerrarProcesso(void);  /* Marca um processo como FINALIZADO          */
void alterarEstado(void);     /* Altera manualmente o estado de um processo */
void exibirProcessos(void);   /* Exibe a tabela de processos                */

/* ---- Acesso usado por outros modulos ---- */
Processo* obterTabelaProcessos(int *quantidade); /* Ponteiro p/ a tabela + total */
Processo* buscarProcessoPorPid(int pid);         /* NULL se nao existir          */
const char* estadoParaTexto(Estado e);           /* "READY", "RUNNING", ...      */

#endif /* PROCESSOS_H */
