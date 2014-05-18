NAME=uManage
SOURCES=$(NAME).c config.c idle.c winmgmt.c
UISOURCES=$(SOURCES) indicate.c
CC=gcc
RM=rm
GLUE=ruby ../GladeGlue/GladeGlue.rb
CFLAGS=-Wall -Wextra -std=c89 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -rdynamic
CFLAGS+=`pkg-config --cflags glib-2.0 gtk+-3.0 appindicator3-0.1`
LIBFLAGS=-lxdo -lX11 -lXss -lxcb -lxcb-screensaver -lpthread
LIBFLAGS+=`pkg-config --libs glib-2.0 gtk+-3.0 appindicator3-0.1`
DBGFLAGS=-g

all: uManage uManageUi

uManage: $(SOURCES) uManage.h config.h idle.h winmgmt.h
	$(CC) -o$(NAME) $(SOURCES) $(CFLAGS) $(LIBFLAGS)

uManageUi: $(UISOURCES) uManage.h config.h idle.h winmgmt.h indicate.h umenu_glade
	$(CC) -DGUI -o$(NAME)Ui $(UISOURCES) umenu_glade.c $(CFLAGS) $(LIBFLAGS)

umenu_glade: umenu.glade
	$(GLUE) umenu

clean:
	$(RM) *~ *_glade.c *_glade.h

