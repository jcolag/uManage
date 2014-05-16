NAME=uManage
SOURCES=$(NAME).c config.c idle.c winmgmt.c
CC=gcc
CFLAGS=-Wall -Wextra -std=c89 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition
CFLAGS+=`pkg-config --cflags glib-2.0`
LIBFLAGS=-lxdo -lX11 -lXss -lxcb -lxcb-screensaver
LIBFLAGS+=`pkg-config --libs glib-2.0`
DBGFLAGS=-g

all: uManage

uManage: uManage.c uManage.h config.c config.h idle.c idle.h winmgmt.c winmgmt.h
	$(CC) -o$(NAME) $(SOURCES) $(CFLAGS) $(LIBFLAGS)

