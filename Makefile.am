EXTRA_DIST = \
		AUTHORS \
		VERSION \
		NEWS \
		COPYING

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = po


plugindir = $(PIDGIN_LIBDIR)/pidgin

plugin_LTLIBRARIES = pidgin-libnotify+.la

pidgin_libnotify__la_LDFLAGS = -module -avoid-version

pidgin_libnotify__la_SOURCES = \
	src/pidgin-libnotify+.c

pidgin_libnotify__la_LIBADD = \
	$(LIBNOTIFY_LIBS)

pidgin_libnotify__la_CFLAGS = \
	-DVERSION=\"$(VERSION)\" \
	-DDATADIR=\"$(PIDGIN_DATADIR)\" \
	-DLOCALEDIR=\"$(PIDGIN_DATADIR)/locale\" \
	$(PIDGIN_CFLAGS) \
	$(LIBNOTIFY_CFLAGS) \
	$(DEBUG_CFLAGS)

