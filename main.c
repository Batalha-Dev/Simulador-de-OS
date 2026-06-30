/* ============================================================================
 *  main.c  -  Simulador de Sistema Operacional
 * ----------------------------------------------------------------------------
 *  Projeto Final de Sistemas Operacionais.
 *
 *  Ponto de entrada do simulador: exibe o menu principal e encaminha o
 *  usuario para cada subsistema (modulo) do sistema operacional simulado:
 *
 *    Modulo 1 - Gerenciamento de Processos      (processos.c)
 *    Modulo 2 - Escalonamento da CPU            (escalonador.c)
 *    Modulo 3 - Gerenciamento de Memoria        (memoria.c)
 *    Modulo 4 - Dispositivos de E/S             (dispositivos.c)
 *    Modulo 5 - Sistema de Arquivos             (arquivos.c)
 *    Modulo 6 - Controle de Concorrencia        (concorrencia.c)
 *    Modulo 7 - Deteccao de Deadlock (desafio)  (deadlock.c)
 * ==========================================================================*/
#include "utils.h"
#include "processos.h"
#include "escalonador.h"
#include "memoria.h"
#include "dispositivos.h"
#include "arquivos.h"
#include "concorrencia.h"
#include "deadlock.h"

/* Exibe o painel consolidado do sistema (estilo "tempo real"), reunindo
 * processos, memoria e dispositivos em uma unica tela. Tambem mostra as
 * estatisticas da ultima execucao do escalonador. */
static void exibirEstatisticas(void)
{
    limparTela();

    /* ---- Resumo de processos por estado ---- */
    int qtd;
    Processo *tab = obterTabelaProcessos(&qtd);
    int cont[5] = {0, 0, 0, 0, 0};
    for (int i = 0; i < qtd; i++) {
        cont[tab[i].estado]++;
    }

    cabecalho("ESTATISTICAS / PAINEL DO SISTEMA");

    printf("PROCESSOS\n");
    printf("---------------------------------------------\n");
    printf("Total: %d | NEW: %d | READY: %d | RUNNING: %d | "
           "WAITING: %d | TERMINATED: %d\n\n",
           qtd, cont[NOVO], cont[PRONTO], cont[EXECUTANDO],
           cont[BLOQUEADO], cont[FINALIZADO]);

    /* ---- Memoria ---- */
    int total, usada, livre;
    obterUsoMemoria(&total, &usada, &livre);
    printf("MEMORIA\n");
    printf("---------------------------------------------\n");
    printf("Total: %d MB | Usada: %d MB | Livre: %d MB | Uso: %.1f%%\n\n",
           total, usada, livre, total > 0 ? (100.0 * usada / total) : 0.0);

    /* ---- Sistema de arquivos ---- */
    printf("ARQUIVOS\n");
    printf("---------------------------------------------\n");
    printf("Arquivos armazenados: %d\n\n", totalArquivos());

    /* ---- Dispositivos ---- */
    exibirDispositivos();
    printf("\n");

    /* ---- Estatisticas do escalonador (ultima execucao) ---- */
    Estatisticas e = obterEstatisticas();
    printf("ESCALONADOR (ultima execucao)\n");
    printf("---------------------------------------------\n");
    if (e.valida) {
        printf("Algoritmo            : %s\n", e.algoritmo);
        printf("Processos escalonados: %d\n", e.numProcessos);
        printf("Tempo medio de espera: %.2f\n", e.tempoMedioEspera);
        printf("Tempo medio de retorno: %.2f\n", e.tempoMedioRetorno);
        printf("Throughput           : %.4f processos/unidade de tempo\n",
               e.throughput);
    } else {
        printf("Nenhum escalonamento executado ainda.\n");
    }
}

/* Imprime o menu principal do simulador. */
static void exibirMenuPrincipal(void)
{
    limparTela();
    printf("=========================================\n");
    printf("  SIMULADOR DE SISTEMA OPERACIONAL\n");
    printf("=========================================\n");
    printf("1 - Gerenciar Processos\n");
    printf("2 - Executar Escalonador\n");
    printf("3 - Exibir Processos\n");
    printf("4 - Gerenciar Memoria\n");
    printf("5 - Dispositivos de E/S\n");
    printf("6 - Sistema de Arquivos\n");
    printf("7 - Estatisticas / Painel\n");
    printf("8 - Concorrencia (Threads/Mutex)\n");
    printf("9 - Deadlock (Deteccao)\n");
    printf("0 - Sair\n");
    printf("=========================================\n");
}

int main(void)
{
    /* Inicializa os subsistemas que possuem estado pre-definido. */
    inicializarMemoria();
    inicializarDispositivos();

    int opcao;
    do {
        exibirMenuPrincipal();
        opcao = lerInteiro("Escolha uma opcao: ");

        switch (opcao) {
            case 1: menuProcessos();    break;
            case 2: menuEscalonador();  break;
            case 3:
                limparTela();
                exibirProcessos();
                pausar();
                break;
            case 4: menuMemoria();      break;
            case 5: menuDispositivos(); break;
            case 6: menuArquivos();     break;
            case 7:
                exibirEstatisticas();
                pausar();
                break;
            case 8: menuConcorrencia(); break;
            case 9: menuDeadlock();     break;
            case 0:
                printf("\nEncerrando o simulador. Ate logo!\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
                pausar();
                break;
        }
    } while (opcao != 0);

    return 0;
}
