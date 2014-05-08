NAME=uManage
CC=gcc
CFLAGS=-Wall -Wextra -std=c89 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition
LIBFLAGS=-lxdo

all: uManage

uManage: uManage.c uManage.h
	$(CC) -o$(NAME) $(NAME).c $(CFLAGS) $(LIBFLAGS)

