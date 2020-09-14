
# makefile for gbookshelf

PROG = gbookshelf
LIBS = libs/liblist.c libs/libstream.c libs/libstring.c libs/libmbs.c libs/libdpa.c libs/libmd5.c libs/libcmd.c libs/libmdfa.c
SRCS = gbs_genre.c gbs_publisher.c gbs_format.c gbs_language.c gbs_book.c gbs_db.c 
UIS	 = gbs_genre_ui.c gbs_publisher_ui.c gbs_format_ui.c gbs_language_ui.c gbs_book_ui.c main.c
TPS = tps/sqlite3/sqlite3.c

OBJS = $(LIBS:%.c=%.o) $(SRCS:%.c=%.o) $(TPS:%.c=%.o) $(UIS:%.c=%.o) 

GBS_MAJOR_VERSION = 0
GBS_MINOR_VERSION = 1
GBS_MICRO_VERSION = 0
GBS_BUILD_VERSION = 32

GTKSDK=D:/Application/gtk+
GTKSDKINC=$(GTKSDK)/include
GTKSDKLIB=$(GTKSDK)/lib
PKGCONFIG=$(GTKSDK)/bin/pkg-config.exe

#GTKSDKCFLAGS=$(shell $(PKGCONFIG) gtk+-2.0 --cflags)
#GTKSDKLFLAGS=$(shell $(PKGCONFIG) gtk+-2.0 --libs)

#CC = gcc
CC = i686-w64-mingw32-gcc.exe
#CC = x86_64-w64-mingw32-gcc.exe

CFLAGS = -I. -Itps/sqlite3 -Ilibs -D__GNU_SOURCE
CFLAGS += -DGBS_MAJOR_VERSION=$(GBS_MAJOR_VERSION)
CFLAGS += -DGBS_MINOR_VERSION=$(GBS_MINOR_VERSION)
CFLAGS += -DGBS_MICRO_VERSION=$(GBS_MICRO_VERSION)
CFLAGS += -DGBS_BUILD_VERSION=$(GBS_BUILD_VERSION)

WIN_CFLAGS = -mms-bitfields -mwindows
GTK_CFLAGS = -I$(GTKSDK)/include/gtk-2.0 -I$(GTKSDK)/lib/gtk-2.0/include -I$(GTKSDK)/include/atk-1.0 -I$(GTKSDK)/include/cairo -I$(GTKSDK)/include/gdk-pixbuf-2.0 -I$(GTKSDK)/include/pango-1.0 -I$(GTKSDK)/include/glib-2.0 -I$(GTKSDK)/lib/glib-2.0/include -I$(GTKSDK)/include -I$(GTKSDK)/include/freetype2 -I$(GTKSDK)/include/libpng14
LFLAGS += -L$(GTKSDK)/lib -lgtk-win32-2.0 -lgdk-win32-2.0 -latk-1.0 -lgio-2.0 -lpangowin32-1.0 -lgdi32 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lpango-1.0 -lcairo -lgobject-2.0 -lgmodule-2.0 -lgthread-2.0 -lglib-2.0 -lintl -mwindows -std=gnu99

DFLAGS = -g -O2 -Wall #-DGBS_DEBUG_ENABLE

default: $(ICON_HEADER_FILE) $(PROG)

%.o:%.c $(ICON_HEADER_FILE)
	$(CC) -o $@ $(CFLAGS) $(WIN_CFLAGS) $(GTK_CFLAGS) $(DFLAGS) -c $<

$(PROG): $(OBJS)
	$(CC) -o $@ $+ $(LFLAGS)

ICON_HEADER_FILE = gbs_icons.h
ICONS = $(wildcard images/gbs_*.png)

ICON_HEADER_FILE: $(ICONS)
	@echo generating gbs_icons.h ...
	@echo "#ifndef _GBS_ICONS_H_" > $@
	@echo "#define _GBS_ICONS_H_" >> $@
	@for i in $(ICONS); do \
		$(GTKSDK)/bin/gdk-pixbuf-csource --raw --name=`echo $$i | sed 's/^.*\/gbs_/gbs_/g' | sed 's/\.png//g'`_raw $$i >> $@; \
	done
	@echo "#endif" >> $@

clean:
	rm -rf $(PROG) $(OBJS)

dist: clean
	-mkdir gbookshelf-$(GBS_MAJOR_VERSION).$(GBS_MINOR_VERSION).$(GBS_MICRO_VERSION)build$(GBS_BUILD_VERSION)
	cp -a *.[ch] Makefile win32 images gbookshelf-$(GBS_MAJOR_VERSION).$(GBS_MINOR_VERSION).$(GBS_MICRO_VERSION)build$(GBS_BUILD_VERSION)
	tar czvf gbookshelf-$(GBS_MAJOR_VERSION).$(GBS_MINOR_VERSION).$(GBS_MICRO_VERSION)build$(GBS_BUILD_VERSION)-src.tar.gz gbookshelf-$(GBS_MAJOR_VERSION).$(GBS_MINOR_VERSION).$(GBS_MICRO_VERSION)build$(GBS_BUILD_VERSION)
	rm -rf gbookshelf-$(GBS_MAJOR_VERSION).$(GBS_MINOR_VERSION).$(GBS_MICRO_VERSION)build$(GBS_BUILD_VERSION)

release: $(PROG)
	-mkdir gbookshelf-$(GBS_MAJOR_VERSION).$(GBS_MINOR_VERSION).$(GBS_MICRO_VERSION)build$(GBS_BUILD_VERSION)
	cp gbookshelf.exe gbookshelf-$(GBS_MAJOR_VERSION).$(GBS_MINOR_VERSION).$(GBS_MICRO_VERSION)build$(GBS_BUILD_VERSION)
	cp -a win32/*.dll gbookshelf-$(GBS_MAJOR_VERSION).$(GBS_MINOR_VERSION).$(GBS_MICRO_VERSION)build$(GBS_BUILD_VERSION)
	tar czvf gbookshelf-$(GBS_MAJOR_VERSION).$(GBS_MINOR_VERSION).$(GBS_MICRO_VERSION)build$(GBS_BUILD_VERSION)-bin.tar.gz gbookshelf-$(GBS_MAJOR_VERSION).$(GBS_MINOR_VERSION).$(GBS_MICRO_VERSION)build$(GBS_BUILD_VERSION)
	rm -rf gbookshelf-$(GBS_MAJOR_VERSION).$(GBS_MINOR_VERSION).$(GBS_MICRO_VERSION)build$(GBS_BUILD_VERSION)

debug:
	G_SLICE=always-malloc G_DEBUG=gc-friendly valgrind --leak-check=full ./gbookshelf

cproto:
	cproto -E 0 -q -e $(CFLAGS) $(SRCS) 2>/dev/null

.PHONY: clean dist tags
