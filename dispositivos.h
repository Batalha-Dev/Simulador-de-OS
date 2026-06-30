/* ============================================================================
 *  dispositivos.h  -  Modulo 4: Gerenciamento de Dispositivos de E/S
 * ----------------------------------------------------------------------------
 *  Simula dispositivos compartilhados (Impressora, Scanner, Disco) que so
 *  podem ser usados por um processo de cada vez (exclusao mutua).
 *  Quando um dispositivo esta ocupado, os pedidos entram em uma fila de
 *  espera (FIFO), demonstrando bloqueio de processos.
 * ==========================================================================*/
#ifndef DISPOSITIVOS_H
#define DISPOSITIVOS_H

#define MAX_FILA        50  /* Tamanho maximo da fila de espera por dispositivo */
#define NUM_DISPOSITIVOS 3  /* Impressora, Scanner, Disco                       */

/* Estrutura de um dispositivo de E/S. */
typedef struct {
    char nome[30];          /* Nome do dispositivo                */
    int  ocupado;           /* 1 = em uso, 0 = livre              */
    int  pid;               /* PID do processo que o utiliza (-1) */
    int  fila[MAX_FILA];    /* Fila de PIDs aguardando            */
    int  inicioFila;        /* Indice do primeiro da fila         */
    int  fimFila;           /* Indice do proximo slot livre       */
    int  qtdFila;           /* Quantidade de processos na fila    */
} Dispositivo;

/* Inicializa os tres dispositivos como livres e com filas vazias. */
void inicializarDispositivos(void);

/* Submenu de dispositivos. */
void menuDispositivos(void);

/* Operacoes. */
void solicitarDispositivo(void); /* Pede o uso de um dispositivo */
void liberarDispositivo(void);   /* Libera; entrega ao proximo da fila */
void exibirDispositivos(void);   /* Mostra estado e filas */

#endif /* DISPOSITIVOS_H */
