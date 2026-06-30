/* ============================================================================
 *  deadlock.h  -  Modulo 7: Deteccao de Deadlock (Desafio)
 * ----------------------------------------------------------------------------
 *  Simula situacoes de impasse (deadlock) entre processos que disputam
 *  dispositivos. Constroi o GRAFO DE ESPERA (wait-for graph) e detecta a
 *  presenca de ciclos - a condicao classica para deadlock.
 *
 *  Cenario classico (do enunciado):
 *      P1 possui Impressora   e espera Scanner
 *      P2 possui Scanner      e espera Impressora
 *      => P1 -> P2 -> P1  (ciclo)  => DEADLOCK
 * ==========================================================================*/
#ifndef DEADLOCK_H
#define DEADLOCK_H

/* Submenu do modulo de deadlock. */
void menuDeadlock(void);

/* Executa o cenario classico que provoca deadlock e o detecta. */
void cenarioComDeadlock(void);

/* Executa um cenario sem ciclo (sem deadlock), para comparacao. */
void cenarioSemDeadlock(void);

#endif /* DEADLOCK_H */
