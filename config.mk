CC=cc
CFLAGS=-Wall -Wextra -pedantic -std=c99
LIBS=-lws2_32
SRC=main.c
OBJ=$(SRC:.c=.o)