EXTRA_DIST = \
	COPYING \
	purple-libnotify+.sym

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = po

AM_CFLAGS += \
	-DLOCALEDIR=\"$(localedir)\" \
	--include config.h

plugin_LTLIBRARIES = purple-libnotify+.la

purple_libnotify__la_SOURCES = \
	src/purple-libnotify+-frames.h \
	src/purple-libnotify+-utils.h \
	src/purple-libnotify+-common.h \
	src/purple-libnotify+.h \
	src/purple-libnotify+-frames.c \
	src/purple-libnotify+-utils.c \
	src/purple-libnotify+.c

purple_libnotify__la_CFLAGS = \
	$(AM_CFLAGS) \
	$(PURPLE_CFLAGS) \
	$(GDK_PIXBUF_CFLAGS) \
	$(NOTIFY_CFLAGS)

purple_libnotify__la_LDFLAGS = \
	-module -avoid-version \
	-export-symbols $(srcdir)/purple-libnotify+.sym

purple_libnotify__la_LIBADD = \
	$(PURPLE_LIBS) \
	$(GDK_PIXBUF_LIBS) \
	$(NOTIFY_LIBS)

# Hooks
install-data-hook: la-files-removal

# *.la files cleanup
la-files-removal:
	cd $(DESTDIR)/$(pluginsdir) && \
		rm $(plugins_LTLIBRARIES)
