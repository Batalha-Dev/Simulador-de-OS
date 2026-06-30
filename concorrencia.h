/* ============================================================================
 *  concorrencia.h  -  Modulo 6: Controle de Concorrencia
 * ----------------------------------------------------------------------------
 *  Demonstra o acesso simultaneo a um recurso compartilhado (a impressora)
 *  usando threads POSIX (pthread) e exclusao mutua (pthread_mutex_t).
 *
 *  Conceitos demonstrados:
 *    - Threads
 *    - Regiao critica
 *    - Mutex (exclusao mutua)
 *    - Condicao de corrida (race condition)
 * ==========================================================================*/
#ifndef CONCORRENCIA_H
#define CONCORRENCIA_H

/* Submenu do modulo de concorrencia. */
void menuConcorrencia(void);

/* Demonstracao 1: varios processos (threads) disputando a impressora,
 * protegidos por mutex (sem intercalar a saida). */
void demoImpressoraMutex(void);

/* Demonstracao 2: condicao de corrida - mostra o resultado de incrementar
 * um contador compartilhado SEM e COM mutex. */
void demoCondicaoCorrida(void);

#endif /* CONCORRENCIA_H */
