SOURCES=camera-main.c\
        camera-ui2-window.c\
        camera-ui2-gconf-settings.c\
        camera-ui2-settings-dialog.c\
        camera-ui2-helper.c\
        camera-interface.c\
        dbus-helper.c\
        geotagging-helper.c\
        storage-helper.c\
        pmdw.c\
        camera-ui2-preview-window.c

OBJS=camera-main.o\
     camera-ui2-window.o\
     camera-ui2-gconf-settings.o\
     camera-ui2-settings-dialog.o\
     camera-ui2-helper.o\
     camera-interface.o\
     pmdw.o\
     dbus-helper.o\
     geotagging-helper.o\
     storage-helper.o\
     camera-ui2-preview-window.o

BUILDDIR=../build
BIN=$(BUILDDIR)/camera-ui
CC=gcc
PKG_FLAGS=$(shell pkg-config glib-2.0 hildon-1 libhildondesktop-1 libosso gdigicam-0.3 gdigicam-gst-camerabin-0.3 mce sharingdialog liblocation navigation hal gnome-vfs-2.0 --libs --cflags)
CCFLAGS=-DGST_USE_UNSTABLE_API

all:$(BIN) $(LIB)

$(BIN):$(OBJS)
	$(CC) $(CCFLAGS) $(PKG_FLAGS) $(OBJS) -o $(BIN)

.c.o:
	$(CC) $(CCFLAGS) $(PKG_FLAGS) -c $< -o $@

.PHONE: clean all

clean:
	rm $(OBJS)
