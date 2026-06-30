/* ============================================================================
 *  concorrencia.c  -  Implementacao do Modulo 6: Controle de Concorrencia
 * ----------------------------------------------------------------------------
 *  Compilar com a flag -pthread (ver Makefile).
 * ==========================================================================*/
#include "concorrencia.h"
#include "utils.h"
#include <pthread.h>

#define NUM_THREADS  5        /* Quantidade de processos concorrentes      */
#define INCREMENTOS  1000000  /* Incrementos por thread (demo de corrida)  */

/* ---- Recursos compartilhados ---- */
static pthread_mutex_t mutexImpressora;   /* Protege a "impressora"        */
static pthread_mutex_t mutexContador;     /* Protege o contador com mutex  */
static long contadorSemMutex = 0;         /* Modificado sem protecao       */
static long contadorComMutex = 0;         /* Modificado com protecao       */

/* Argumento passado a cada thread (identifica o processo). */
typedef struct {
    int id;   /* numero do processo/thread */
} ArgThread;

/* -------------------------------------------------------------------------
 *  Demonstracao 1: impressora protegida por mutex.
 *  Cada thread entra na regiao critica, "imprime" e sai. Como ha exclusao
 *  mutua, as mensagens nunca se intercalam.
 * ---------------------------------------------------------------------- */
static void* tarefaImpressora(void *arg)
{
    ArgThread *a = (ArgThread *) arg;

    /* Inicio da regiao critica: somente uma thread por vez. */
    pthread_mutex_lock(&mutexImpressora);

    printf("  >> Processo %d ENTROU na regiao critica (usando a impressora)\n",
           a->id);

    /* Simula o tempo de impressao com um pequeno trabalho ocupado. */
    volatile long espera = 0;
    for (long i = 0; i < 30000000L; i++) {
        espera += i;
    }
    (void) espera; /* evita aviso de variavel nao utilizada */

    printf("  << Processo %d terminou de imprimir e LIBEROU a impressora\n",
           a->id);

    pthread_mutex_unlock(&mutexImpressora);
    /* Fim da regiao critica. */

    return NULL;
}

/* Thread que incrementa o contador SEM protecao (gera condicao de corrida). */
static void* incrementaSemMutex(void *arg)
{
    (void) arg; /* nao usado */
    for (int i = 0; i < INCREMENTOS; i++) {
        contadorSemMutex++;     /* operacao NAO atomica -> corrida */
    }
    return NULL;
}

/* Thread que incrementa o contador COM protecao (sem corrida). */
static void* incrementaComMutex(void *arg)
{
    (void) arg;
    for (int i = 0; i < INCREMENTOS; i++) {
        pthread_mutex_lock(&mutexContador);
        contadorComMutex++;     /* regiao critica protegida */
        pthread_mutex_unlock(&mutexContador);
    }
    return NULL;
}

/* Demonstracao da impressora compartilhada com exclusao mutua. */
void demoImpressoraMutex(void)
{
    cabecalho("CONCORRENCIA - IMPRESSORA COM MUTEX");
    printf("%d processos tentarao usar a impressora simultaneamente.\n",
           NUM_THREADS);
    printf("Gracas ao mutex, apenas um por vez entra na regiao critica.\n\n");

    pthread_t threads[NUM_THREADS];
    ArgThread args[NUM_THREADS];

    pthread_mutex_init(&mutexImpressora, NULL);

    /* Cria as threads (processos concorrentes). */
    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].id = i + 1;
        pthread_create(&threads[i], NULL, tarefaImpressora, &args[i]);
    }

    /* Aguarda todas terminarem. */
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutexImpressora);
    printf("\nTodos os processos usaram a impressora sem conflito.\n");
}

/* Demonstracao da condicao de corrida (com e sem mutex). */
void demoCondicaoCorrida(void)
{
    cabecalho("CONCORRENCIA - CONDICAO DE CORRIDA");
    printf("%d threads incrementam um contador %d vezes cada.\n",
           NUM_THREADS, INCREMENTOS);
    long esperado = (long) NUM_THREADS * INCREMENTOS;
    printf("Valor correto esperado: %ld\n\n", esperado);

    pthread_t threads[NUM_THREADS];

    /* ---- Caso 1: SEM mutex (deve dar resultado errado) ---- */
    contadorSemMutex = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, incrementaSemMutex, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("SEM mutex -> contador = %ld  ", contadorSemMutex);
    if (contadorSemMutex != esperado) {
        printf("(ERRADO! perdeu %ld incrementos por causa da corrida)\n",
               esperado - contadorSemMutex);
    } else {
        printf("(coincidiu desta vez, mas nao e garantido)\n");
    }

    /* ---- Caso 2: COM mutex (resultado sempre correto) ---- */
    contadorComMutex = 0;
    pthread_mutex_init(&mutexContador, NULL);
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, incrementaComMutex, NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mutexContador);
    printf("COM mutex -> contador = %ld  (CORRETO)\n", contadorComMutex);
}

/* Submenu do modulo de concorrencia. */
void menuConcorrencia(void)
{
    int opcao;
    do {
        limparTela();
        cabecalho("CONTROLE DE CONCORRENCIA (THREADS / MUTEX)");
        printf("1 - Impressora com exclusao mutua (mutex)\n");
        printf("2 - Demonstrar condicao de corrida (com x sem mutex)\n");
        printf("0 - Voltar\n");
        opcao = lerInteiro("Escolha: ");

        switch (opcao) {
            case 1: demoImpressoraMutex();  pausar(); break;
            case 2: demoCondicaoCorrida();  pausar(); break;
            case 0: break;
            default:
                printf("Opcao invalida.\n");
                pausar();
                break;
        }
    } while (opcao != 0);
}
