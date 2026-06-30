/* ============================================================================
 *  memoria.c  -  Implementacao do Modulo 3: Gerenciamento de Memoria
 * ==========================================================================*/
#include "memoria.h"
#include "processos.h"
#include "utils.h"

/* ---- Estado interno do modulo ---- */
static BlocoMemoria memoria[MAX_BLOCOS]; /* Lista de blocos contiguos     */
static int numBlocos = 0;                /* Quantidade de blocos na lista */
static AlgoritmoFit algoritmoAtual = FIRST_FIT; /* Algoritmo de alocacao  */

/* Nome textual do algoritmo atual (para exibicao). */
static const char* nomeAlgoritmo(void)
{
    switch (algoritmoAtual) {
        case FIRST_FIT: return "First Fit";
        case BEST_FIT:  return "Best Fit";
        case WORST_FIT: return "Worst Fit";
        default:        return "?";
    }
}

/* Inicializa a RAM como um unico grande bloco livre. */
void inicializarMemoria(void)
{
    numBlocos = 1;
    memoria[0].inicio      = 0;
    memoria[0].tamanho     = MEMORIA_TOTAL;
    memoria[0].livre       = 1;
    memoria[0].pid         = -1;
    memoria[0].requisitado = 0;
}

/* Arredonda o tamanho pedido para cima ate um multiplo da unidade de
 * alocacao. A diferenca entre o pedido e o alocado e a fragmentacao interna. */
static int arredondarTamanho(int pedido)
{
    int resto = pedido % UNIDADE_ALOCACAO;
    if (resto == 0) {
        return pedido;
    }
    return pedido + (UNIDADE_ALOCACAO - resto);
}

/* Insere um novo bloco na posicao 'pos', deslocando os demais a direita. */
static void inserirBloco(int pos, BlocoMemoria novo)
{
    for (int i = numBlocos; i > pos; i--) {
        memoria[i] = memoria[i - 1];
    }
    memoria[pos] = novo;
    numBlocos++;
}

/* Remove o bloco na posicao 'pos', deslocando os demais a esquerda. */
static void removerBloco(int pos)
{
    for (int i = pos; i < numBlocos - 1; i++) {
        memoria[i] = memoria[i + 1];
    }
    numBlocos--;
}

/* Escolhe o indice do bloco livre a ser usado, conforme o algoritmo atual.
 * Retorna -1 se nenhum bloco livre comportar 'tamanho'. */
static int escolherBloco(int tamanho)
{
    int escolhido = -1;

    for (int i = 0; i < numBlocos; i++) {
        if (!memoria[i].livre || memoria[i].tamanho < tamanho) {
            continue; /* ocupado ou pequeno demais */
        }

        if (escolhido == -1) {
            escolhido = i;
            continue;
        }

        switch (algoritmoAtual) {
            case FIRST_FIT:
                /* Ja temos o primeiro; nada a fazer. */
                return escolhido;
            case BEST_FIT:
                if (memoria[i].tamanho < memoria[escolhido].tamanho) {
                    escolhido = i;
                }
                break;
            case WORST_FIT:
                if (memoria[i].tamanho > memoria[escolhido].tamanho) {
                    escolhido = i;
                }
                break;
        }
    }
    return escolhido;
}

/* Aloca memoria para um processo, de forma interativa. */
void alocarMemoria(void)
{
    if (numBlocos >= MAX_BLOCOS) {
        printf("Limite de blocos atingido. Libere memoria antes de alocar.\n");
        return;
    }

    int pid = lerInteiro("PID do processo que recebera memoria: ");
    Processo *p = buscarProcessoPorPid(pid);
    if (p == NULL) {
        printf("Aviso: nao existe processo com PID %d na tabela.\n", pid);
        printf("A alocacao prosseguira usando o PID como rotulo.\n");
    }

    int pedido = lerInteiro("Quantidade de memoria desejada (MB): ");
    if (pedido <= 0) {
        printf("Tamanho invalido.\n");
        return;
    }

    int tamReal = arredondarTamanho(pedido);
    int idx = escolherBloco(tamReal);

    if (idx == -1) {
        /* Pode haver memoria livre suficiente, mas fragmentada (externa). */
        int total, usada, livre;
        obterUsoMemoria(&total, &usada, &livre);
        printf("\nFalha na alocacao de %d MB (arredondado para %d MB).\n",
               pedido, tamReal);
        if (livre >= tamReal) {
            printf(">> FRAGMENTACAO EXTERNA: ha %d MB livres no total, mas\n",
                   livre);
            printf("   espalhados em varios buracos. Nenhum bloco unico cabe.\n");
        } else {
            printf(">> Memoria insuficiente (livre = %d MB).\n", livre);
        }
        return;
    }

    /* Bloco escolhido: aloca tamReal e deixa o restante como novo bloco livre. */
    BlocoMemoria *b = &memoria[idx];
    int sobra = b->tamanho - tamReal;

    b->tamanho     = tamReal;
    b->livre       = 0;
    b->pid         = pid;
    b->requisitado = pedido;

    if (sobra > 0) {
        BlocoMemoria livreNovo;
        livreNovo.inicio      = b->inicio + tamReal;
        livreNovo.tamanho     = sobra;
        livreNovo.livre       = 1;
        livreNovo.pid         = -1;
        livreNovo.requisitado = 0;
        inserirBloco(idx + 1, livreNovo);
    }

    printf("\nAlocados %d MB (pedido: %d MB) para o processo %d via %s.\n",
           tamReal, pedido, pid, nomeAlgoritmo());
    if (tamReal != pedido) {
        printf(">> FRAGMENTACAO INTERNA: %d MB desperdicados dentro do bloco.\n",
               tamReal - pedido);
    }
}

/* Funde blocos livres adjacentes em um unico bloco maior. */
static void fundirBlocosLivres(void)
{
    for (int i = 0; i < numBlocos - 1; ) {
        if (memoria[i].livre && memoria[i + 1].livre) {
            memoria[i].tamanho += memoria[i + 1].tamanho;
            removerBloco(i + 1);
            /* Nao incrementa i: pode haver mais blocos livres em sequencia. */
        } else {
            i++;
        }
    }
}

/* Libera toda a memoria pertencente a um processo. */
void liberarMemoria(void)
{
    int pid = lerInteiro("PID do processo cuja memoria sera liberada: ");
    int liberou = 0;

    for (int i = 0; i < numBlocos; i++) {
        if (!memoria[i].livre && memoria[i].pid == pid) {
            memoria[i].livre       = 1;
            memoria[i].pid         = -1;
            memoria[i].requisitado = 0;
            liberou = 1;
        }
    }

    if (!liberou) {
        printf("Nenhum bloco pertencente ao processo %d foi encontrado.\n", pid);
        return;
    }

    fundirBlocosLivres();
    printf("Memoria do processo %d liberada com sucesso.\n", pid);
}

/* Exibe o mapa de memoria de forma linear e detalhada. */
void exibirMapaMemoria(void)
{
    cabecalho("MAPA DE MEMORIA");
    printf("Algoritmo atual: %s\n", nomeAlgoritmo());
    printf("Memoria Total: %d MB\n\n", MEMORIA_TOTAL);

    /* Linha esquematica: [P1 - 200 MB] [Livre - 100 MB] ... */
    for (int i = 0; i < numBlocos; i++) {
        if (memoria[i].livre) {
            printf("[Livre - %d MB] ", memoria[i].tamanho);
        } else {
            printf("[P%d - %d MB] ", memoria[i].pid, memoria[i].tamanho);
        }
    }
    printf("\n\n");

    /* Tabela detalhada. */
    printf("%-8s %-8s %-10s %-6s %-10s\n",
           "Inicio", "Tam(MB)", "Estado", "PID", "FragInt");
    printf("------------------------------------------------\n");

    int fragInternaTotal = 0;
    for (int i = 0; i < numBlocos; i++) {
        int fragInt = 0;
        char pidStr[8];

        if (memoria[i].livre) {
            strcpy(pidStr, "-");
        } else {
            fragInt = memoria[i].tamanho - memoria[i].requisitado;
            fragInternaTotal += fragInt;
            sprintf(pidStr, "%d", memoria[i].pid);
        }

        printf("%-8d %-8d %-10s %-6s %-10d\n",
               memoria[i].inicio,
               memoria[i].tamanho,
               memoria[i].livre ? "LIVRE" : "OCUPADO",
               pidStr,
               fragInt);
    }

    /* Estatisticas de fragmentacao. */
    int total, usada, livre;
    obterUsoMemoria(&total, &usada, &livre);

    int buracos = 0, maiorBuraco = 0;
    for (int i = 0; i < numBlocos; i++) {
        if (memoria[i].livre) {
            buracos++;
            if (memoria[i].tamanho > maiorBuraco) {
                maiorBuraco = memoria[i].tamanho;
            }
        }
    }

    printf("------------------------------------------------\n");
    printf("Usada: %d MB | Livre: %d MB | Blocos livres: %d\n",
           usada, livre, buracos);
    printf("Maior bloco livre contiguo: %d MB\n", maiorBuraco);
    printf("Fragmentacao interna total: %d MB\n", fragInternaTotal);
    if (buracos > 1) {
        printf("Fragmentacao externa: %d MB livres espalhados em %d buracos.\n",
               livre, buracos);
    }
}

/* Calcula o uso atual da memoria (em MB). */
void obterUsoMemoria(int *total, int *usada, int *livre)
{
    int u = 0, l = 0;
    for (int i = 0; i < numBlocos; i++) {
        if (memoria[i].livre) {
            l += memoria[i].tamanho;
        } else {
            u += memoria[i].tamanho;
        }
    }
    if (total) *total = MEMORIA_TOTAL;
    if (usada) *usada = u;
    if (livre) *livre = l;
}

/* Submenu de gerenciamento de memoria. */
void menuMemoria(void)
{
    int opcao;
    do {
        limparTela();
        cabecalho("GERENCIAR MEMORIA");
        printf("Algoritmo atual: %s\n\n", nomeAlgoritmo());
        printf("1 - Escolher algoritmo (First/Best/Worst Fit)\n");
        printf("2 - Alocar memoria para processo\n");
        printf("3 - Liberar memoria de processo\n");
        printf("4 - Exibir mapa de memoria\n");
        printf("0 - Voltar\n");
        opcao = lerInteiro("Escolha: ");

        switch (opcao) {
            case 1: {
                printf("\n0 - First Fit | 1 - Best Fit | 2 - Worst Fit\n");
                int alg = lerInteiro("Algoritmo: ");
                if (alg >= 0 && alg <= 2) {
                    algoritmoAtual = (AlgoritmoFit) alg;
                    printf("Algoritmo definido para %s.\n", nomeAlgoritmo());
                } else {
                    printf("Opcao invalida.\n");
                }
                pausar();
                break;
            }
            case 2: alocarMemoria();     pausar(); break;
            case 3: liberarMemoria();    pausar(); break;
            case 4: exibirMapaMemoria(); pausar(); break;
            case 0: break;
            default:
                printf("Opcao invalida.\n");
                pausar();
                break;
        }
    } while (opcao != 0);
}
