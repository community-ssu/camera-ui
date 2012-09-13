DESKTOP_FILE_PATH=$(shell pkg-config osso-af-settings --variable=desktopentrydir)
DBUS_SERVICE_DIR=$(shell pkg-config osso-af-settings --variable=dbusservicedir)
EXEC_DIR=/usr/bin
SUBDIRS= src
BUILDDIR=build
BIN=camera-ui

all: create_builddir subdirs

install: all
	install -d $(DESTDIR)/$(EXEC_DIR)
	install -m 755 $(BUILDDIR)/$(BIN) $(DESTDIR)/$(EXEC_DIR)
	install -d $(DESTDIR)/$(DESKTOP_FILE_PATH)
	install -m 644 data/camera-ui.desktop $(DESTDIR)/$(DESKTOP_FILE_PATH)
	install -d $(DESTDIR)/$(DBUS_SERVICE_DIR)
	install -m 644 data/com.nokia.camera-ui.service $(DESTDIR)/$(DBUS_SERVICE_DIR)
	install -d $(DESTDIR)/etc/X11/Xsession.post
	install -m 644 data/17camera-ui $(DESTDIR)/etc/X11/Xsession.post/
	install -d $(DESTDIR)/etc/sudoers.d
	install -m 644 data/camera-ui.sudoers $(DESTDIR)/etc/sudoers.d
	install -d $(DESTDIR)/usr/bin
	install -m 755 data/camera-ui-set-priority $(DESTDIR)/usr/bin
	install -d $(DESTDIR)/usr/share/icons/hicolor/48x48/hildon
	install -m 644 data/48x48/camera_iso1600.png $(DESTDIR)/usr/share/icons/hicolor/48x48/hildon/
	install -m 644 data/48x48/camera_iso800.png $(DESTDIR)/usr/share/icons/hicolor/48x48/hildon/
	install -m 644 data/48x48/camera_isoAuto.png $(DESTDIR)/usr/share/icons/hicolor/48x48/hildon/
	install -m 644 data/48x48/camera_iso1600_pressed.png $(DESTDIR)/usr/share/icons/hicolor/48x48/hildon/
	install -m 644 data/48x48/camera_iso800_pressed.png $(DESTDIR)/usr/share/icons/hicolor/48x48/hildon/
	install -m 644 data/48x48/camera_isoAuto_pressed.png $(DESTDIR)/usr/share/icons/hicolor/48x48/hildon/
	install -d $(DESTDIR)/usr/share/icons/hicolor/32x32/hildon
	install -m 644 data/32x32/camera_iso1600.png $(DESTDIR)/usr/share/icons/hicolor/32x32/hildon/
	install -m 644 data/32x32/camera_iso800.png $(DESTDIR)/usr/share/icons/hicolor/32x32/hildon/
	install -m 644 data/32x32/camera_isoAuto.png $(DESTDIR)/usr/share/icons/hicolor/32x32/hildon/

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

create_builddir:
	mkdir -p build

.PHONY: all clean install $(SUBDIRS)

clean:
	rm -rf build
	for d in $(SUBDIRS); do (cd $$d; $(MAKE) clean); done
