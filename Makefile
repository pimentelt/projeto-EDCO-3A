# =================================================
#   SISTEMA LOGISTICO - UBERDRONES v2.0
#   Makefile
# =================================================

# Detecta o sistema operacional
ifdef OS
	OS := $(strip $(OS))
else
	OS := $(strip $(shell uname))
endif

CC     = gcc
CFLAGS = -Wall -Wextra -std=c99

SRC = uberdrones.c
OBJ = uberdrones.o

# Configuracoes por sistema
ifeq ($(OS),Windows_NT)
	BIN = uberdrones.exe
	RM  = del /Q /F
	RUN = ./uberdrones.exe
else
	BIN = uberdrones
	RM  = rm -f
	RUN = ./uberdrones
endif

# Regra principal
all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $(BIN)

uberdrones.o: uberdrones.c
	$(CC) $(CFLAGS) -c uberdrones.c

# Executar o programa
run:
	$(RUN)

# Limpar arquivos gerados
clean:
	$(RM) $(OBJ) $(BIN)

# Recompilar do zero
rebuild: clean all

# Exibir ajuda
help:
	@echo Alvos disponiveis:
	@echo   make         - compila o projeto
	@echo   make run     - executa o programa
	@echo   make clean   - remove arquivos gerados
	@echo   make rebuild - limpa e recompila do zero
	@echo   make help    - exibe esta mensagem

.PHONY: all run clean rebuild help
