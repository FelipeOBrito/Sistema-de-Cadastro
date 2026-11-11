# Makefile do Cadastro de Loja em C
# Uso:  make        compila
#       make run    compila e executa
#       make test   roda os testes automaticos
#       make clean  apaga o executavel e arquivos temporarios

CC     = gcc
CFLAGS = -Wall -Wextra -std=c99

all: loja

loja: loja.c
	$(CC) $(CFLAGS) -o loja loja.c

run: loja
	./loja

test:
	bash tests/run_tests.sh

clean:
	rm -f loja loja.exe temp.csv

.PHONY: all run test clean
