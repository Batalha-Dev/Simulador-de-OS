/* ============================================================================
 *  arquivos.h  -  Modulo 5: Sistema de Arquivos Simplificado
 * ----------------------------------------------------------------------------
 *  Simula o armazenamento de arquivos usando um vetor (organizacao simples).
 *  Operacoes: criar, remover, listar e buscar arquivos.
 * ==========================================================================*/
#ifndef ARQUIVOS_H
#define ARQUIVOS_H

#define MAX_ARQUIVOS 100  /* Numero maximo de arquivos no sistema */

/* Estrutura que representa um arquivo. */
typedef struct {
    char nome[50];  /* Nome do arquivo (ex.: "relatorio.pdf") */
    int  tamanho;   /* Tamanho do arquivo (KB)                 */
} Arquivo;

/* Submenu do sistema de arquivos. */
void menuArquivos(void);

/* Operacoes. */
void criarArquivo(void);   /* Cria um novo arquivo            */
void removerArquivo(void); /* Remove um arquivo pelo nome     */
void listarArquivos(void); /* Lista todos os arquivos         */
void buscarArquivo(void);  /* Busca um arquivo pelo nome      */

/* Quantidade de arquivos (para Estatisticas). */
int totalArquivos(void);

#endif /* ARQUIVOS_H */
