/* ============================================================================
 *  processos.c  -  Implementacao do Modulo 1: Gerenciamento de Processos
 * ==========================================================================*/
#include "processos.h"
#include "utils.h"

/* ---- Estado interno do modulo (tabela global de processos) ---- */
static Processo tabela[MAX_PROCESSOS]; /* Vetor com todos os processos    */
static int totalProcessos = 0;         /* Quantidade atual de processos   */
static int proximoPid = 1;             /* Gerador de PIDs sequenciais     */
static int contadorChegada = 0;        /* Ordem de chegada dos processos  */

/* Converte o enum de estado em texto legivel (estilo dos SOs reais). */
const char* estadoParaTexto(Estado e)
{
    switch (e) {
        case NOVO:       return "NEW";
        case PRONTO:     return "READY";
        case EXECUTANDO: return "RUNNING";
        case BLOQUEADO:  return "WAITING";
        case FINALIZADO: return "TERMINATED";
        default:         return "DESCONHECIDO";
    }
}

/* Retorna o ponteiro para a tabela e, por referencia, a quantidade de itens.
 * Permite que outros modulos percorram os processos sem copiar a tabela. */
Processo* obterTabelaProcessos(int *quantidade)
{
    if (quantidade != NULL) {
        *quantidade = totalProcessos;
    }
    return tabela;
}

/* Procura um processo pelo PID. Retorna NULL se nao encontrar. */
Processo* buscarProcessoPorPid(int pid)
{
    for (int i = 0; i < totalProcessos; i++) {
        if (tabela[i].pid == pid) {
            return &tabela[i];
        }
    }
    return NULL;
}

/* Cria um novo processo com base nos dados informados pelo usuario.
 * Todo processo nasce no estado NOVO e logo passa para PRONTO. */
void criarProcesso(void)
{
    if (totalProcessos >= MAX_PROCESSOS) {
        printf("Tabela de processos cheia! Nao e possivel criar mais.\n");
        return;
    }

    Processo p;
    p.pid = proximoPid++;
    lerTexto("Nome do processo: ", p.nome, sizeof(p.nome));
    if (p.nome[0] == '\0') {
        /* Garante um nome padrao caso o usuario deixe em branco. */
        sprintf(p.nome, "Proc%d", p.pid);
    }
    p.prioridade    = lerInteiro("Prioridade (1 = mais alta): ");
    p.tempoExecucao = lerInteiro("Tempo de execucao (unidades de CPU): ");
    p.tempoChegada  = contadorChegada++;

    /* Ciclo de vida: NOVO -> PRONTO (pronto para ser escalonado). */
    p.estado = NOVO;
    tabela[totalProcessos++] = p;
    tabela[totalProcessos - 1].estado = PRONTO;

    printf("\nProcesso criado com sucesso!\n");
    printf("PID atribuido: %d  |  Estado: %s\n",
           p.pid, estadoParaTexto(PRONTO));
}

/* Encerra um processo: muda seu estado para FINALIZADO. */
void encerrarProcesso(void)
{
    if (totalProcessos == 0) {
        printf("Nao ha processos cadastrados.\n");
        return;
    }

    int pid = lerInteiro("Informe o PID do processo a encerrar: ");
    Processo *p = buscarProcessoPorPid(pid);

    if (p == NULL) {
        printf("Processo com PID %d nao encontrado.\n", pid);
        return;
    }
    if (p->estado == FINALIZADO) {
        printf("O processo %d ja esta FINALIZADO.\n", pid);
        return;
    }

    p->estado = FINALIZADO;
    printf("Processo %d (%s) encerrado. Estado: %s\n",
           p->pid, p->nome, estadoParaTexto(p->estado));
}

/* Permite alterar manualmente o estado de um processo (uso didatico). */
void alterarEstado(void)
{
    if (totalProcessos == 0) {
        printf("Nao ha processos cadastrados.\n");
        return;
    }

    int pid = lerInteiro("Informe o PID do processo: ");
    Processo *p = buscarProcessoPorPid(pid);

    if (p == NULL) {
        printf("Processo com PID %d nao encontrado.\n", pid);
        return;
    }

    printf("\nEstados disponiveis:\n");
    printf("  0 - NEW (Novo)\n");
    printf("  1 - READY (Pronto)\n");
    printf("  2 - RUNNING (Executando)\n");
    printf("  3 - WAITING (Bloqueado)\n");
    printf("  4 - TERMINATED (Finalizado)\n");

    int novo = lerInteiro("Novo estado: ");
    if (novo < 0 || novo > 4) {
        printf("Opcao de estado invalida.\n");
        return;
    }

    p->estado = (Estado) novo;
    printf("Estado do processo %d alterado para %s.\n",
           p->pid, estadoParaTexto(p->estado));
}

/* Exibe a tabela de processos em formato tabular. */
void exibirProcessos(void)
{
    cabecalho("PROCESSOS");

    if (totalProcessos == 0) {
        printf("Nenhum processo cadastrado.\n");
        return;
    }

    printf("%-5s %-15s %-6s %-8s %-12s\n",
           "PID", "Nome", "Prio", "Tempo", "Estado");
    printf("--------------------------------------------------\n");

    for (int i = 0; i < totalProcessos; i++) {
        printf("%-5d %-15s %-6d %-8d %-12s\n",
               tabela[i].pid,
               tabela[i].nome,
               tabela[i].prioridade,
               tabela[i].tempoExecucao,
               estadoParaTexto(tabela[i].estado));
    }
}

/* Submenu de gerenciamento de processos (Modulo 1). */
void menuProcessos(void)
{
    int opcao;
    do {
        limparTela();
        cabecalho("GERENCIAR PROCESSOS");
        printf("1 - Criar processo\n");
        printf("2 - Encerrar processo\n");
        printf("3 - Alterar estado\n");
        printf("4 - Exibir tabela de processos\n");
        printf("0 - Voltar\n");
        opcao = lerInteiro("Escolha: ");

        switch (opcao) {
            case 1: criarProcesso();    pausar(); break;
            case 2: encerrarProcesso(); pausar(); break;
            case 3: alterarEstado();    pausar(); break;
            case 4: exibirProcessos();  pausar(); break;
            case 0: break;
            default:
                printf("Opcao invalida.\n");
                pausar();
                break;
        }
    } while (opcao != 0);
}
