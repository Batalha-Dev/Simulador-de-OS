# ============================================================================
#  Makefile - Simulador de Sistema Operacional
# ----------------------------------------------------------------------------
#  Uso:
#    make            -> compila o projeto (gera o executavel)
#    make run        -> compila e executa
#    make clean      -> remove objetos e executavel
#
#  No Windows (MinGW) use 'mingw32-make' no lugar de 'make'.
# ============================================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -g
LDFLAGS = -pthread

# Detecta o sistema operacional (nome do executavel e comando de limpeza).
ifeq ($(OS),Windows_NT)
    EXE = simulador.exe
    RM  = cmd /c del /Q
    LDFLAGS += -static
else
    EXE = simulador
    RM  = rm -f
endif

# Arquivos-fonte e objetos.
SRCS    = main.c processos.c escalonador.c memoria.c dispositivos.c \
          arquivos.c concorrencia.c deadlock.c utils.c
OBJS    = $(SRCS:.c=.o)
HEADERS = $(wildcard *.h)

# Alvo padrao: compila tudo.
all: $(EXE)

# Liga os objetos no executavel final.
$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJS) $(LDFLAGS)

# Compila cada .c em .o (recompila se qualquer cabecalho mudar).
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Compila e executa.
run: $(EXE)
	./$(EXE)

# Remove arquivos gerados (o '-' ignora erro caso algum arquivo nao exista).
clean:
	-$(RM) $(OBJS) $(EXE)

.PHONY: all run clean
