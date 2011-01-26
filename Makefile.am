EXTRA_DIST = \
		AUTHORS \
		VERSION \
		NEWS \
		COPYING

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = po


plugindir = $(PIDGIN_LIBDIR)/pidgin

plugin_LTLIBRARIES = pidgin-libnotify+.la

pidgin_libnotify__la_SOURCES = \
	src/pidgin-libnotify+-frames.c \
	src/pidgin-libnotify+-utils.c \
	src/pidgin-libnotify+.c

pidgin_libnotify__la_CFLAGS = \
	-DVERSION=\"$(VERSION)\" \
	-DDATADIR=\"$(PIDGIN_DATADIR)\" \
	-DLOCALEDIR=\"$(PIDGIN_DATADIR)/locale\" \
	$(PIDGIN_CFLAGS) \
	$(LIBNOTIFY_CFLAGS) \
	$(DEBUG_CFLAGS)

pidgin_libnotify__la_LDFLAGS = -module -avoid-version -export-symbols $(srcdir)/pidgin-libnotify+.sym

pidgin_libnotify__la_LIBADD = \
	$(LIBNOTIFY_LIBS)

