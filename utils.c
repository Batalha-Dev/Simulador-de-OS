/* ============================================================================
 *  utils.c  -  Implementacao das funcoes utilitarias
 * ==========================================================================*/
#include "utils.h"

/* Limpa a tela de acordo com o sistema operacional hospedeiro. */
void limparTela(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* Aguarda o usuario pressionar ENTER para continuar. */
void pausar(void)
{
    printf("\nPressione ENTER para continuar...");
    /* Consome caracteres pendentes ate a quebra de linha. */
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* descarta */ }
}

/* Imprime um cabecalho padronizado, centralizando visualmente o titulo. */
void cabecalho(const char *titulo)
{
    printf("=========================================\n");
    printf("  %s\n", titulo);
    printf("=========================================\n");
}

/* Le um inteiro de forma robusta: se o usuario digitar algo invalido,
 * limpa o buffer e pede novamente. */
int lerInteiro(const char *mensagem)
{
    int valor;
    int lido;
    int c;

    while (1) {
        printf("%s", mensagem);
        lido = scanf("%d", &valor);

        if (lido == 1) {
            /* Descarta o resto da linha (inclusive o '\n'). */
            while ((c = getchar()) != '\n' && c != EOF) { /* descarta */ }
            return valor;
        }

        if (lido == EOF) {
            /* Fluxo de entrada encerrado (ex.: Ctrl+Z): sai com seguranca. */
            printf("\nEntrada encerrada. Saindo do simulador.\n");
            exit(0);
        }

        /* Entrada invalida: limpa o buffer e avisa. */
        while ((c = getchar()) != '\n' && c != EOF) { /* descarta */ }
        printf("Entrada invalida. Digite um numero inteiro.\n");
    }
}

/* Le uma linha completa de texto (aceitando espacos). */
void lerTexto(const char *mensagem, char *destino, int tamanho)
{
    printf("%s", mensagem);

    if (fgets(destino, tamanho, stdin) != NULL) {
        /* Remove o '\n' final, se existir. */
        size_t len = strlen(destino);
        if (len > 0 && destino[len - 1] == '\n') {
            destino[len - 1] = '\0';
        }
    } else {
        destino[0] = '\0';
    }
}
