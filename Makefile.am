EXTRA_DIST = \
	AUTHORS \
	VERSION \
	NEWS \
	COPYING \
	pidgin-libnotify+.sym

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = po


plugindir = $(PIDGIN_LIBDIR)/pidgin

plugin_LTLIBRARIES = pidgin-libnotify+.la

pidgin_libnotify__la_SOURCES = \
	src/pidgin-libnotify+-frames.h \
	src/pidgin-libnotify+-utils.h \
	src/pidgin-libnotify+-common.h \
	src/pidgin-libnotify+.h \
	src/pidgin-libnotify+-frames.c \
	src/pidgin-libnotify+-utils.c \
	src/pidgin-libnotify+.c

pidgin_libnotify__la_CFLAGS = \
	-DVERSION=\"$(VERSION)\" \
	-DDATADIR=\"$(PIDGIN_DATADIR)\" \
	-DLOCALEDIR=\"$(localedir)\" \
	$(PIDGIN_CFLAGS) \
	$(LIBNOTIFY_CFLAGS) \
	$(DEBUG_CFLAGS)

pidgin_libnotify__la_LDFLAGS = -module -avoid-version -export-symbols $(srcdir)/pidgin-libnotify+.sym

pidgin_libnotify__la_LIBADD = \
	$(LIBNOTIFY_LIBS)
