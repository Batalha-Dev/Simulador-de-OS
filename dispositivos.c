/* ============================================================================
 *  dispositivos.c  -  Implementacao do Modulo 4: Dispositivos de E/S
 * ==========================================================================*/
#include "dispositivos.h"
#include "processos.h"
#include "utils.h"

/* Vetor com os dispositivos do sistema. */
static Dispositivo dispositivos[NUM_DISPOSITIVOS];

/* Inicializa os dispositivos: Impressora, Scanner e Disco. */
void inicializarDispositivos(void)
{
    const char *nomes[NUM_DISPOSITIVOS] = { "Impressora", "Scanner", "Disco" };

    for (int i = 0; i < NUM_DISPOSITIVOS; i++) {
        strcpy(dispositivos[i].nome, nomes[i]);
        dispositivos[i].ocupado    = 0;
        dispositivos[i].pid        = -1;
        dispositivos[i].inicioFila = 0;
        dispositivos[i].fimFila    = 0;
        dispositivos[i].qtdFila    = 0;
    }
}

/* Enfileira um PID na fila de espera do dispositivo (operacao FIFO). */
static void enfileirar(Dispositivo *d, int pid)
{
    if (d->qtdFila >= MAX_FILA) {
        printf("Fila do %s cheia. Pedido descartado.\n", d->nome);
        return;
    }
    d->fila[d->fimFila] = pid;
    d->fimFila = (d->fimFila + 1) % MAX_FILA;
    d->qtdFila++;
}

/* Remove e retorna o primeiro PID da fila. Retorna -1 se vazia. */
static int desenfileirar(Dispositivo *d)
{
    if (d->qtdFila == 0) {
        return -1;
    }
    int pid = d->fila[d->inicioFila];
    d->inicioFila = (d->inicioFila + 1) % MAX_FILA;
    d->qtdFila--;
    return pid;
}

/* Mostra o menu de selecao de dispositivo e retorna o indice (-1 cancela). */
static int selecionarDispositivo(void)
{
    printf("\nDispositivos:\n");
    for (int i = 0; i < NUM_DISPOSITIVOS; i++) {
        printf("  %d - %s\n", i + 1, dispositivos[i].nome);
    }
    int op = lerInteiro("Escolha o dispositivo (0 cancela): ");
    if (op < 1 || op > NUM_DISPOSITIVOS) {
        return -1;
    }
    return op - 1;
}

/* Solicita o uso de um dispositivo para um processo. */
void solicitarDispositivo(void)
{
    int idx = selecionarDispositivo();
    if (idx == -1) {
        printf("Operacao cancelada.\n");
        return;
    }

    int pid = lerInteiro("PID do processo solicitante: ");
    Processo *p = buscarProcessoPorPid(pid);
    if (p == NULL) {
        printf("Aviso: processo %d nao existe na tabela (sera usado como rotulo).\n",
               pid);
    }

    Dispositivo *d = &dispositivos[idx];

    if (!d->ocupado) {
        /* Dispositivo livre: concede o uso imediatamente (exclusao mutua). */
        d->ocupado = 1;
        d->pid     = pid;
        printf("\n%s concedido ao processo %d.\n", d->nome, pid);
        if (p != NULL) {
            p->estado = EXECUTANDO; /* usando o recurso */
        }
    } else {
        /* Ocupado: o processo entra na fila e fica BLOQUEADO. */
        enfileirar(d, pid);
        printf("\n%s ocupado pelo processo %d.\n", d->nome, d->pid);
        printf("Processo %d colocado na fila de espera (posicao %d).\n",
               pid, d->qtdFila);
        if (p != NULL) {
            p->estado = BLOQUEADO; /* aguardando recurso */
        }
    }
}

/* Libera um dispositivo; se houver fila, entrega ao proximo processo. */
void liberarDispositivo(void)
{
    int idx = selecionarDispositivo();
    if (idx == -1) {
        printf("Operacao cancelada.\n");
        return;
    }

    Dispositivo *d = &dispositivos[idx];
    if (!d->ocupado) {
        printf("O %s ja esta livre.\n", d->nome);
        return;
    }

    printf("\nProcesso %d liberou o %s.\n", d->pid, d->nome);

    /* O processo que liberou volta a PRONTO. */
    Processo *anterior = buscarProcessoPorPid(d->pid);
    if (anterior != NULL && anterior->estado != FINALIZADO) {
        anterior->estado = PRONTO;
    }

    /* Entrega o dispositivo ao proximo da fila, se houver. */
    int proximo = desenfileirar(d);
    if (proximo != -1) {
        d->ocupado = 1;
        d->pid     = proximo;
        printf("%s concedido ao proximo da fila: processo %d.\n",
               d->nome, proximo);
        Processo *p = buscarProcessoPorPid(proximo);
        if (p != NULL) {
            p->estado = EXECUTANDO;
        }
    } else {
        d->ocupado = 0;
        d->pid     = -1;
        printf("%s agora esta livre.\n", d->nome);
    }
}

/* Exibe o estado de todos os dispositivos e suas filas. */
void exibirDispositivos(void)
{
    cabecalho("DISPOSITIVOS DE E/S");

    for (int i = 0; i < NUM_DISPOSITIVOS; i++) {
        Dispositivo *d = &dispositivos[i];
        printf("%-12s -> ", d->nome);
        if (d->ocupado) {
            printf("PID %d", d->pid);
        } else {
            printf("Livre");
        }

        /* Imprime a fila de espera (na ordem FIFO). */
        if (d->qtdFila > 0) {
            printf("  | Fila: ");
            int idx = d->inicioFila;
            for (int k = 0; k < d->qtdFila; k++) {
                printf("P%d ", d->fila[idx]);
                idx = (idx + 1) % MAX_FILA;
            }
        }
        printf("\n");
    }
}

/* Submenu de dispositivos. */
void menuDispositivos(void)
{
    int opcao;
    do {
        limparTela();
        cabecalho("DISPOSITIVOS DE E/S");
        printf("1 - Solicitar dispositivo\n");
        printf("2 - Liberar dispositivo\n");
        printf("3 - Exibir dispositivos e filas\n");
        printf("0 - Voltar\n");
        opcao = lerInteiro("Escolha: ");

        switch (opcao) {
            case 1: solicitarDispositivo(); pausar(); break;
            case 2: liberarDispositivo();   pausar(); break;
            case 3: exibirDispositivos();   pausar(); break;
            case 0: break;
            default:
                printf("Opcao invalida.\n");
                pausar();
                break;
        }
    } while (opcao != 0);
}
