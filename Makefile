NAME=uManage
SOURCES=$(NAME).c config.c idle.c winmgmt.c sqlite.c
UISOURCES=$(SOURCES) indicate.c uoptions.c
CC=gcc
RM=rm
GLUE=GladeGlue.rb
CFLAGS=-Wall -Wextra -std=c89 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition -rdynamic
LIBS=glib-2.0 gtk+-3.0 appindicator3-0.1 sqlite3
CFLAGS+=`pkg-config --cflags $(LIBS)` -g
LIBFLAGS=-lxdo -lX11 -lXss -lxcb -lxcb-screensaver -lpthread
LIBFLAGS+=`pkg-config --libs $(LIBS)`
DBGFLAGS=-g

all: uManage uManageUi

uManage: $(SOURCES) uManage.h config.h idle.h winmgmt.h sqlite.h
	$(CC) -o$(NAME) $(SOURCES) $(CFLAGS) $(LIBFLAGS)

uManageUi: $(UISOURCES) uManage.h config.h idle.h winmgmt.h indicate.h sqlite.h umenu_glade opts_glade
	$(CC) -DGUI -o$(NAME)Ui $(UISOURCES) umenu_glade.c uoptions_glade.c $(CFLAGS) $(LIBFLAGS)

umenu_glade: umenu.glade
	$(GLUE) umenu

opts_glade: uoptions.glade
	$(GLUE) uoptions

clean:
	$(RM) *~ *_glade.c *_glade.h

