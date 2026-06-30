/* ============================================================================
 *  utils.h  -  Funcoes utilitarias compartilhadas pelo simulador
 * ----------------------------------------------------------------------------
 *  Reune funcoes de uso geral (limpar tela, pausar, ler entrada do usuario
 *  de forma segura e desenhar cabecalhos) usadas por todos os modulos.
 * ==========================================================================*/
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Limpa o terminal (cls no Windows, clear em Unix/Linux). */
void limparTela(void);

/* Pausa a execucao ate o usuario pressionar ENTER. */
void pausar(void);

/* Desenha um cabecalho padronizado entre linhas de '='. */
void cabecalho(const char *titulo);

/* Le um numero inteiro do teclado com validacao.
 * Reexibe a mensagem ate receber um inteiro valido. */
int lerInteiro(const char *mensagem);

/* Le uma linha de texto (com espacos) para 'destino', sem o '\n' final.
 * 'tamanho' e o tamanho do buffer de destino. */
void lerTexto(const char *mensagem, char *destino, int tamanho);

#endif /* UTILS_H */
