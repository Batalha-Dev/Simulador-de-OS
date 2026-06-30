/* ============================================================================
 *  deadlock.c  -  Implementacao do Modulo 7: Deteccao de Deadlock
 * ==========================================================================*/
#include "deadlock.h"
#include "utils.h"

#define MAX_PROC 10  /* Numero maximo de processos no cenario   */
#define MAX_REC  10  /* Numero maximo de recursos no cenario    */

/* Estado de um cenario de alocacao de recursos. */
typedef struct {
    int numProc;                 /* Quantidade de processos              */
    int numRec;                  /* Quantidade de recursos               */
    char nomeRec[MAX_REC][30];   /* Nome de cada recurso                 */
    int dono[MAX_REC];           /* Indice do processo que possui o rec. (-1 livre) */
    int espera[MAX_PROC];        /* Recurso que o processo espera (-1 = nenhum)     */
} Cenario;

/* Grafo de espera (wait-for): grafo[i][j] = 1 se Pi espera por Pj. */
static int grafo[MAX_PROC][MAX_PROC];

/* Cores para a busca em profundidade (deteccao de ciclo). */
enum { BRANCO, CINZA, PRETO };
static int cor[MAX_PROC];
static int caminho[MAX_PROC]; /* pilha do caminho atual (para mostrar o ciclo) */
static int topo;

/* Constroi o grafo de espera a partir do cenario.
 * Pi -> Pj quando Pi espera um recurso cujo dono e Pj. */
static void construirGrafo(const Cenario *c)
{
    for (int i = 0; i < c->numProc; i++) {
        for (int j = 0; j < c->numProc; j++) {
            grafo[i][j] = 0;
        }
    }

    for (int p = 0; p < c->numProc; p++) {
        int rec = c->espera[p];
        if (rec >= 0) {
            int donoRec = c->dono[rec];
            if (donoRec >= 0 && donoRec != p) {
                grafo[p][donoRec] = 1; /* p espera por donoRec */
            }
        }
    }
}

/* Exibe o grafo de espera como lista de arestas. */
static void exibirGrafo(const Cenario *c)
{
    printf("\nGrafo de espera (wait-for):\n");
    int houve = 0;
    for (int i = 0; i < c->numProc; i++) {
        for (int j = 0; j < c->numProc; j++) {
            if (grafo[i][j]) {
                printf("  P%d --espera--> P%d\n", i + 1, j + 1);
                houve = 1;
            }
        }
    }
    if (!houve) {
        printf("  (nenhuma dependencia de espera entre processos)\n");
    }
}

/* DFS para detectar ciclo a partir do vertice u.
 * Retorna 1 se encontrar ciclo e preenche 'caminho' com o ciclo. */
static int dfsCiclo(int u, int n)
{
    cor[u] = CINZA;
    caminho[topo++] = u;

    for (int v = 0; v < n; v++) {
        if (!grafo[u][v]) {
            continue;
        }
        if (cor[v] == CINZA) {
            /* Aresta de retorno -> ciclo encontrado. */
            caminho[topo++] = v; /* fecha o ciclo para exibicao */
            return 1;
        }
        if (cor[v] == BRANCO && dfsCiclo(v, n)) {
            return 1;
        }
    }

    cor[u] = PRETO;
    topo--;
    return 0;
}

/* Detecta deadlock procurando ciclo no grafo de espera. */
static int detectarDeadlock(const Cenario *c)
{
    for (int i = 0; i < c->numProc; i++) {
        cor[i] = BRANCO;
    }

    for (int i = 0; i < c->numProc; i++) {
        if (cor[i] == BRANCO) {
            topo = 0;
            if (dfsCiclo(i, c->numProc)) {
                return 1;
            }
        }
    }
    return 0;
}

/* Exibe a alocacao atual de recursos (quem possui o que e quem espera). */
static void exibirAlocacao(const Cenario *c)
{
    printf("Alocacao de recursos:\n");
    for (int r = 0; r < c->numRec; r++) {
        if (c->dono[r] >= 0) {
            printf("  %s -> P%d\n", c->nomeRec[r], c->dono[r] + 1);
        } else {
            printf("  %s -> Livre\n", c->nomeRec[r]);
        }
    }
    printf("\nPedidos pendentes:\n");
    for (int p = 0; p < c->numProc; p++) {
        if (c->espera[p] >= 0) {
            printf("  P%d espera por %s\n", p + 1, c->nomeRec[c->espera[p]]);
        }
    }
}

/* Roda um cenario completo: exibe alocacao, grafo e o resultado da deteccao. */
static void executarCenario(const Cenario *c, const char *titulo)
{
    cabecalho(titulo);
    exibirAlocacao(c);
    construirGrafo(c);
    exibirGrafo(c);

    if (detectarDeadlock(c)) {
        printf("\n>>> RESULTADO: DEADLOCK DETECTADO! <<<\n");
        printf("Ciclo de espera: ");
        for (int i = 0; i < topo; i++) {
            printf("P%d", caminho[i] + 1);
            if (i < topo - 1) printf(" -> ");
        }
        printf("\n");
    } else {
        printf("\n>>> RESULTADO: nenhum deadlock detectado. <<<\n");
    }
}

/* Cenario classico do enunciado: gera deadlock. */
void cenarioComDeadlock(void)
{
    Cenario c;
    c.numProc = 2;
    c.numRec  = 2;
    strcpy(c.nomeRec[0], "Impressora");
    strcpy(c.nomeRec[1], "Scanner");

    /* P1 (indice 0) possui Impressora; P2 (indice 1) possui Scanner. */
    c.dono[0] = 0; /* Impressora -> P1 */
    c.dono[1] = 1; /* Scanner    -> P2 */

    /* P1 espera Scanner; P2 espera Impressora. */
    c.espera[0] = 1; /* P1 espera Scanner    */
    c.espera[1] = 0; /* P2 espera Impressora */

    executarCenario(&c, "DEADLOCK - CENARIO CLASSICO");
}

/* Cenario sem ciclo: nao ha deadlock. */
void cenarioSemDeadlock(void)
{
    Cenario c;
    c.numProc = 2;
    c.numRec  = 2;
    strcpy(c.nomeRec[0], "Impressora");
    strcpy(c.nomeRec[1], "Scanner");

    c.dono[0] = 0;   /* Impressora -> P1 */
    c.dono[1] = 1;   /* Scanner    -> P2 */

    /* P1 espera Scanner, mas P2 nao espera nada -> sem ciclo. */
    c.espera[0] = 1; /* P1 espera Scanner */
    c.espera[1] = -1;/* P2 nao espera     */

    executarCenario(&c, "SEM DEADLOCK - CENARIO SEGURO");
}

/* Submenu do modulo de deadlock. */
void menuDeadlock(void)
{
    int opcao;
    do {
        limparTela();
        cabecalho("DEADLOCK (DETECCAO)");
        printf("1 - Cenario com deadlock (classico)\n");
        printf("2 - Cenario sem deadlock (comparacao)\n");
        printf("0 - Voltar\n");
        opcao = lerInteiro("Escolha: ");

        switch (opcao) {
            case 1: cenarioComDeadlock(); pausar(); break;
            case 2: cenarioSemDeadlock(); pausar(); break;
            case 0: break;
            default:
                printf("Opcao invalida.\n");
                pausar();
                break;
        }
    } while (opcao != 0);
}
