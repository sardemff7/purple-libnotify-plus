EXTRA_DIST = \
	COPYING \
	purple-libnotify+.sym

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = po


plugindir = $(PURPLE_LIBDIR)/purple-2

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
	-DDATADIR=\"$(PURPLE_DATADIR)\" \
	-DLOCALEDIR=\"$(localedir)\" \
	$(PURPLE_CFLAGS) \
	$(GDK_PIXBUF_CFLAGS) \
	$(LIBNOTIFY_CFLAGS) \
	$(DEBUG_CFLAGS)

purple_libnotify__la_LDFLAGS = -module -avoid-version -export-symbols $(srcdir)/purple-libnotify+.sym

purple_libnotify__la_LIBADD = \
	$(PURPLE_LIBS) \
	$(GDK_PIXBUF_LIBS) \
	$(LIBNOTIFY_LIBS)
