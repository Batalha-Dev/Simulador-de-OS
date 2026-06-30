/* ============================================================================
 *  escalonador.c  -  Implementacao do Modulo 2: Escalonamento da CPU
 * ==========================================================================*/
#include "escalonador.h"
#include "processos.h"
#include "utils.h"

/* Estrutura auxiliar usada apenas durante a simulacao do escalonamento.
 * Trabalhamos sobre copias para nao corromper a tabela real ate o fim. */
typedef struct {
    int  pid;
    char nome[50];
    int  prioridade;
    int  burst;      /* tempo total de CPU (copia de tempoExecucao) */
    int  restante;   /* tempo restante (usado no Round Robin)       */
    int  espera;     /* tempo de espera calculado                   */
    int  retorno;    /* tempo de retorno (turnaround)               */
} ProcSim;

/* Guarda as estatisticas da ultima execucao para o menu de Estatisticas. */
static Estatisticas ultimaExec = { "", 0, 0.0, 0.0, 0.0, 0, 0 };

Estatisticas obterEstatisticas(void)
{
    return ultimaExec;
}

/* Coleta os processos PRONTOS (com tempo de execucao > 0) em um vetor de
 * simulacao. Retorna a quantidade coletada. */
static int coletarProntos(ProcSim *sim)
{
    int qtd;
    Processo *tab = obterTabelaProcessos(&qtd);
    int n = 0;

    for (int i = 0; i < qtd; i++) {
        if (tab[i].estado == PRONTO && tab[i].tempoExecucao > 0) {
            sim[n].pid        = tab[i].pid;
            strcpy(sim[n].nome, tab[i].nome);
            sim[n].prioridade = tab[i].prioridade;
            sim[n].burst      = tab[i].tempoExecucao;
            sim[n].restante   = tab[i].tempoExecucao;
            sim[n].espera     = 0;
            sim[n].retorno    = 0;
            n++;
        }
    }
    return n;
}

/* Apos a simulacao, marca os processos executados como FINALIZADO na
 * tabela real (eles rodaram ate o fim). */
static void finalizarProcessos(ProcSim *sim, int n)
{
    for (int i = 0; i < n; i++) {
        Processo *p = buscarProcessoPorPid(sim[i].pid);
        if (p != NULL) {
            p->estado = FINALIZADO;
        }
    }
}

/* Calcula e armazena as metricas, alem de imprimi-las na tela. */
static void calcularMetricas(ProcSim *sim, int n, int tempoTotal,
                             const char *algoritmo)
{
    double somaEspera = 0.0, somaRetorno = 0.0;

    for (int i = 0; i < n; i++) {
        somaEspera  += sim[i].espera;
        somaRetorno += sim[i].retorno;
    }

    ultimaExec.numProcessos     = n;
    ultimaExec.tempoMedioEspera = (n > 0) ? somaEspera  / n : 0.0;
    ultimaExec.tempoMedioRetorno= (n > 0) ? somaRetorno / n : 0.0;
    ultimaExec.throughput       = (tempoTotal > 0) ? (double) n / tempoTotal : 0.0;
    ultimaExec.tempoTotal       = tempoTotal;
    ultimaExec.valida           = 1;
    strcpy(ultimaExec.algoritmo, algoritmo);

    printf("\n----------------- METRICAS -----------------\n");
    printf("%-5s %-15s %-8s %-8s\n", "PID", "Nome", "Espera", "Retorno");
    for (int i = 0; i < n; i++) {
        printf("%-5d %-15s %-8d %-8d\n",
               sim[i].pid, sim[i].nome, sim[i].espera, sim[i].retorno);
    }
    printf("--------------------------------------------\n");
    printf("Tempo medio de espera : %.2f\n", ultimaExec.tempoMedioEspera);
    printf("Tempo medio de retorno: %.2f\n", ultimaExec.tempoMedioRetorno);
    printf("Throughput            : %.4f processos/unidade de tempo\n",
           ultimaExec.throughput);
    printf("Tempo total de CPU    : %d unidades\n", tempoTotal);
}

/* -------------------------------------------------------------------------
 *  FCFS - First Come First Served
 *  Executa os processos na ordem de chegada (ordem da tabela).
 * ---------------------------------------------------------------------- */
void escalonarFCFS(void)
{
    ProcSim sim[MAX_PROCESSOS];
    int n = coletarProntos(sim);

    cabecalho("ESCALONAMENTO FCFS (First Come First Served)");
    if (n == 0) {
        printf("Nenhum processo PRONTO para escalonar.\n");
        return;
    }

    int tempoAtual = 0;
    /* Em FCFS nao preemptivo: cada processo espera os anteriores terminarem. */
    for (int i = 0; i < n; i++) {
        printf("Executando P%d (%s) por %d unidades...\n",
               sim[i].pid, sim[i].nome, sim[i].burst);

        sim[i].espera  = tempoAtual;                 /* espera = tempo ja decorrido */
        tempoAtual    += sim[i].burst;               /* CPU avanca pelo burst       */
        sim[i].retorno = tempoAtual;                 /* retorno = conclusao (chegada 0) */
    }

    calcularMetricas(sim, n, tempoAtual, "FCFS");
    finalizarProcessos(sim, n);
}

/* -------------------------------------------------------------------------
 *  Round Robin
 *  Cada processo recebe fatias de tempo (quantum) ate concluir.
 * ---------------------------------------------------------------------- */
void escalonarRoundRobin(void)
{
    ProcSim sim[MAX_PROCESSOS];
    int n = coletarProntos(sim);

    cabecalho("ESCALONAMENTO ROUND ROBIN");
    if (n == 0) {
        printf("Nenhum processo PRONTO para escalonar.\n");
        return;
    }

    int quantum = lerInteiro("Defina o Quantum: ");
    if (quantum <= 0) {
        printf("Quantum invalido. Usando quantum = 1.\n");
        quantum = 1;
    }

    int tempoAtual = 0;
    int concluidos = 0;
    printf("\n");

    /* Repete em rodadas ate todos os processos terminarem. */
    while (concluidos < n) {
        int algumExecutou = 0;

        for (int i = 0; i < n; i++) {
            if (sim[i].restante <= 0) {
                continue; /* processo ja concluido */
            }
            algumExecutou = 1;

            /* Executa no maximo 'quantum' unidades. */
            int fatia = (sim[i].restante < quantum) ? sim[i].restante : quantum;
            printf("Executando P%d (%s) por %d unidade(s) "
                   "[restante antes: %d]\n",
                   sim[i].pid, sim[i].nome, fatia, sim[i].restante);

            sim[i].restante -= fatia;
            tempoAtual      += fatia;

            if (sim[i].restante == 0) {
                /* Processo concluiu nesta fatia. */
                sim[i].retorno = tempoAtual;            /* chegada 0 */
                sim[i].espera  = sim[i].retorno - sim[i].burst;
                concluidos++;
                printf("   -> P%d FINALIZADO em t=%d\n", sim[i].pid, tempoAtual);
            }
        }

        if (!algumExecutou) {
            break; /* seguranca contra laco infinito */
        }
    }

    calcularMetricas(sim, n, tempoAtual, "Round Robin");
    finalizarProcessos(sim, n);
}

/* -------------------------------------------------------------------------
 *  Prioridade (nao preemptivo)
 *  Processos com MENOR valor de prioridade executam primeiro.
 * ---------------------------------------------------------------------- */
void escalonarPrioridade(void)
{
    ProcSim sim[MAX_PROCESSOS];
    int n = coletarProntos(sim);

    cabecalho("ESCALONAMENTO POR PRIORIDADE");
    if (n == 0) {
        printf("Nenhum processo PRONTO para escalonar.\n");
        return;
    }

    /* Ordena por prioridade (selection sort - simples e didatico). */
    for (int i = 0; i < n - 1; i++) {
        int menor = i;
        for (int j = i + 1; j < n; j++) {
            if (sim[j].prioridade < sim[menor].prioridade) {
                menor = j;
            }
        }
        if (menor != i) {
            ProcSim tmp = sim[i];
            sim[i] = sim[menor];
            sim[menor] = tmp;
        }
    }

    int tempoAtual = 0;
    for (int i = 0; i < n; i++) {
        printf("Executando P%d (%s) [prioridade %d] por %d unidades...\n",
               sim[i].pid, sim[i].nome, sim[i].prioridade, sim[i].burst);

        sim[i].espera  = tempoAtual;
        tempoAtual    += sim[i].burst;
        sim[i].retorno = tempoAtual;
    }

    calcularMetricas(sim, n, tempoAtual, "Prioridade");
    finalizarProcessos(sim, n);
}

/* Submenu do escalonador. */
void menuEscalonador(void)
{
    int opcao;
    do {
        limparTela();
        cabecalho("ESCALONADOR DE CPU");
        printf("1 - FCFS (First Come First Served)\n");
        printf("2 - Round Robin\n");
        printf("3 - Prioridade\n");
        printf("0 - Voltar\n");
        opcao = lerInteiro("Escolha o algoritmo: ");

        switch (opcao) {
            case 1: escalonarFCFS();       pausar(); break;
            case 2: escalonarRoundRobin(); pausar(); break;
            case 3: escalonarPrioridade(); pausar(); break;
            case 0: break;
            default:
                printf("Opcao invalida.\n");
                pausar();
                break;
        }
    } while (opcao != 0);
}
