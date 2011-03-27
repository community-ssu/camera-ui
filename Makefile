DESKTOP_FILE_PATH=$(shell pkg-config osso-af-settings --variable=desktopentrydir)
DBUS_SERVICE_DIR=$(shell pkg-config osso-af-settings --variable=dbusservicedir)
EXEC_DIR=/usr/bin
SUBDIRS= src
BUILDDIR=build
BIN=camera-ui


all: create_builddir subdirs

install: all
	install -d $(DESTDIR)/$(EXEC_DIR)
	install -m 644 $(BUILDDIR)/$(BIN) $(DESTDIR)/$(EXEC_DIR)
	install -d $(DESTDIR)/$(DESKTOP_FILE_PATH)
	install -m 644 data/camera-ui.desktop $(DESTDIR)/$(DESKTOP_FILE_PATH)
	install -d $(DESTDIR)/$(DBUS_SERVICE_DIR)
	install -m 644 data/com.nokia.camera-ui.service $(DESTDIR)/$(DBUS_SERVICE_DIR)
	install -d $(DESTDIR)/etc/X11/Xsession.post
	install -m 644 data/17camera-ui $(DESTDIR)/etc/X11/Xsession.post/

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

create_builddir:
	mkdir -p build

.PHONY: all clean install $(SUBDIRS)

clean:
	rm -rf build
	for d in $(SUBDIRS); do (cd $$d; $(MAKE) clean); done
