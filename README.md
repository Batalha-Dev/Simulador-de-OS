# Simulador-de-OS
Simulador de Sistema Operacional usando linguagem C


Projeto Final da disciplina de **Sistemas Operacionais**.

Simulador, em linguagem **C**, dos principais subsistemas de um sistema
operacional moderno. O programa é controlado por um **menu textual** no
terminal e reproduz o comportamento dos módulos internos de um SO por meio
de estruturas de dados e algoritmos — sem acessar o hardware real.

---

## 1. Módulos implementados

| # | Módulo | Arquivos | Conteúdo |
|---|--------|----------|----------|
| 1 | Gerenciamento de Processos | `processos.c/.h` | Criação, encerramento, troca de estado e tabela de processos (NEW, READY, RUNNING, WAITING, TERMINATED) |
| 2 | Escalonamento da CPU | `escalonador.c/.h` | **FCFS**, **Round Robin** (quantum) e **Prioridade** + métricas (espera, retorno, throughput) |
| 3 | Gerenciamento de Memória | `memoria.c/.h` | RAM de 1024 MB, **First/Best/Worst Fit**, mapa de memória, fragmentação interna e externa |
| 4 | Dispositivos de E/S | `dispositivos.c/.h` | Impressora, Scanner e Disco com exclusão mútua e fila de espera (FIFO) |
| 5 | Sistema de Arquivos | `arquivos.c/.h` | Criar, remover, listar e buscar arquivos (organização em vetor) |
| 6 | Controle de Concorrência | `concorrencia.c/.h` | `pthread` + `pthread_mutex_t`: impressora protegida e demonstração de condição de corrida |
| 7 | Detecção de Deadlock (desafio) | `deadlock.c/.h` | Grafo de espera (wait-for) e detecção de ciclo |
|   | Utilidades | `utils.c/.h` | Entrada segura, limpeza de tela, cabeçalhos |
|   | Programa principal | `main.c` | Menu principal e painel de estatísticas |

---

## 2. Requisitos

- Compilador **C** com suporte a **POSIX threads (pthread)**.
  - **Windows:** MinGW-w64 (ex.: WinLibs). Use `mingw32-make`.
  - **Linux:** `gcc` e `make` (pacote `build-essential`).
  - **macOS:** Command Line Tools (`xcode-select --install`) — o `clang` faz o papel do `gcc`.

> Guia detalhado de instalação e compilação para os três sistemas (e como gerar o `.exe`): ver **Documentacao_Completa.docx**.

---

## 3. Como compilar

### Com Makefile

```bash
# Linux / macOS
make

# Windows (MinGW)
mingw32-make
```

Isso gera o executável `simulador` (Linux/macOS) ou `simulador.exe` (Windows).

### Manualmente (sem Makefile)

```bash
gcc -Wall -Wextra -g -pthread -o simulador main.c processos.c escalonador.c \
    memoria.c dispositivos.c arquivos.c concorrencia.c deadlock.c utils.c
```

---

## 4. Como executar

```bash
# Linux / macOS
./simulador      # ou: make run

# Windows
.\simulador.exe
```

### Limpar arquivos gerados

```bash
make clean        # ou mingw32-make clean
```

---

## 5. Menu principal

```
=========================================
  SIMULADOR DE SISTEMA OPERACIONAL
=========================================
1 - Gerenciar Processos
2 - Executar Escalonador
3 - Exibir Processos
4 - Gerenciar Memoria
5 - Dispositivos de E/S
6 - Sistema de Arquivos
7 - Estatisticas / Painel
8 - Concorrencia (Threads/Mutex)
9 - Deadlock (Deteccao)
0 - Sair
=========================================
```

---

## 6. Premissas e decisões de projeto

- **Escalonamento:** assume-se que todos os processos PRONTOS chegam no
  instante 0 (modelo clássico de livro-texto); a ordem de chegada é a ordem
  de criação. O *throughput* é calculado como `nº de processos / tempo total`.
- **Prioridade:** menor valor numérico = maior prioridade.
- **Memória:** particionamento dinâmico (variável). As requisições são
  arredondadas para múltiplos da unidade de alocação (`UNIDADE_ALOCACAO`,
  4 MB), o que gera **fragmentação interna**. Blocos livres adjacentes são
  fundidos ao liberar memória.
- **Integração entre módulos:** um processo que está usando um dispositivo
  fica no estado RUNNING/WAITING e, por isso, não é considerado pelo
  escalonador (que só pega processos READY) — comportamento coerente com um
  SO real.

---

## 7. Sugestão de roteiro de teste rápido

1. **Criar processos** (menu 1 → 1): crie 3 processos (ex.: Chrome, VSCode, Spotify).
2. **Exibir processos** (menu 3): confira a tabela.
3. **Memória** (menu 4 → 2): aloque memória para alguns processos e veja o mapa (4 → 4).
4. **Dispositivos** (menu 5 → 1): solicite a impressora para dois processos e veja a fila (5 → 3).
5. **Escalonador** (menu 2): rode FCFS, Round Robin e Prioridade e compare as métricas.
6. **Concorrência** (menu 8 → 2): veja a condição de corrida com e sem mutex.
7. **Deadlock** (menu 9 → 1): observe o ciclo de espera sendo detectado.
8. **Painel** (menu 7): veja o resumo geral do sistema.
