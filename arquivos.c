/* ============================================================================
 *  arquivos.c  -  Implementacao do Modulo 5: Sistema de Arquivos
 * ==========================================================================*/
#include "arquivos.h"
#include "utils.h"

/* ---- Estado interno: vetor de arquivos ---- */
static Arquivo arquivos[MAX_ARQUIVOS];
static int numArquivos = 0;

int totalArquivos(void)
{
    return numArquivos;
}

/* Procura o indice de um arquivo pelo nome. Retorna -1 se nao existir. */
static int indiceArquivo(const char *nome)
{
    for (int i = 0; i < numArquivos; i++) {
        if (strcmp(arquivos[i].nome, nome) == 0) {
            return i;
        }
    }
    return -1;
}

/* Cria um novo arquivo com nome e tamanho informados. */
void criarArquivo(void)
{
    if (numArquivos >= MAX_ARQUIVOS) {
        printf("Sistema de arquivos cheio.\n");
        return;
    }

    Arquivo a;
    lerTexto("Nome do arquivo (ex.: dados.txt): ", a.nome, sizeof(a.nome));
    if (a.nome[0] == '\0') {
        printf("Nome invalido.\n");
        return;
    }
    if (indiceArquivo(a.nome) != -1) {
        printf("Ja existe um arquivo chamado '%s'.\n", a.nome);
        return;
    }

    a.tamanho = lerInteiro("Tamanho do arquivo (KB): ");
    if (a.tamanho < 0) {
        printf("Tamanho invalido.\n");
        return;
    }

    arquivos[numArquivos++] = a;
    printf("Arquivo '%s' (%d KB) criado com sucesso.\n", a.nome, a.tamanho);
}

/* Remove um arquivo pelo nome, deslocando os demais no vetor. */
void removerArquivo(void)
{
    if (numArquivos == 0) {
        printf("Nao ha arquivos para remover.\n");
        return;
    }

    char nome[50];
    lerTexto("Nome do arquivo a remover: ", nome, sizeof(nome));

    int idx = indiceArquivo(nome);
    if (idx == -1) {
        printf("Arquivo '%s' nao encontrado.\n", nome);
        return;
    }

    /* Desloca os arquivos seguintes uma posicao a esquerda. */
    for (int i = idx; i < numArquivos - 1; i++) {
        arquivos[i] = arquivos[i + 1];
    }
    numArquivos--;
    printf("Arquivo '%s' removido.\n", nome);
}

/* Lista todos os arquivos cadastrados. */
void listarArquivos(void)
{
    cabecalho("SISTEMA DE ARQUIVOS");

    if (numArquivos == 0) {
        printf("Nenhum arquivo armazenado.\n");
        return;
    }

    printf("%-30s %-10s\n", "Nome", "Tam(KB)");
    printf("----------------------------------------\n");
    for (int i = 0; i < numArquivos; i++) {
        printf("%-30s %-10d\n", arquivos[i].nome, arquivos[i].tamanho);
    }
    printf("----------------------------------------\n");
    printf("Total: %d arquivo(s).\n", numArquivos);
}

/* Busca um arquivo pelo nome e exibe seus dados. */
void buscarArquivo(void)
{
    if (numArquivos == 0) {
        printf("Nao ha arquivos para buscar.\n");
        return;
    }

    char nome[50];
    lerTexto("Nome do arquivo a buscar: ", nome, sizeof(nome));

    int idx = indiceArquivo(nome);
    if (idx == -1) {
        printf("Arquivo '%s' nao encontrado.\n", nome);
    } else {
        printf("Encontrado: %s (%d KB).\n",
               arquivos[idx].nome, arquivos[idx].tamanho);
    }
}

/* Submenu do sistema de arquivos. */
void menuArquivos(void)
{
    int opcao;
    do {
        limparTela();
        cabecalho("SISTEMA DE ARQUIVOS");
        printf("1 - Criar arquivo\n");
        printf("2 - Remover arquivo\n");
        printf("3 - Listar arquivos\n");
        printf("4 - Buscar arquivo\n");
        printf("0 - Voltar\n");
        opcao = lerInteiro("Escolha: ");

        switch (opcao) {
            case 1: criarArquivo();   pausar(); break;
            case 2: removerArquivo(); pausar(); break;
            case 3: listarArquivos(); pausar(); break;
            case 4: buscarArquivo();  pausar(); break;
            case 0: break;
            default:
                printf("Opcao invalida.\n");
                pausar();
                break;
        }
    } while (opcao != 0);
}
