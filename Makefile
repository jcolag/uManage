NAME=uManage
CC=gcc
CFLAGS=-Wall -Wextra -std=c89 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition
LIBFLAGS=-lxdo -lX11 -lXss -lxcb -lxcb-screensaver
DBGFLAGS=-g

all: uManage

uManage: uManage.c uManage.h
	$(CC) -o$(NAME) $(NAME).c $(CFLAGS) $(LIBFLAGS)

debug: uManage.c uManage.h
	$(CC) $(DBGFLAGS) -o$(NAME) $(NAME).c $(CFLAGS) $(LIBFLAGS)

