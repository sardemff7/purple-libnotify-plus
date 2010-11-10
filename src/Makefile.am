gddir = $(PIDGIN_LIBDIR)/pidgin

pidgin_libnotify__la_LDFLAGS = -module -avoid-version

gd_LTLIBRARIES = pidgin-libnotify+.la

pidgin_libnotify__la_SOURCES = \
	pidgin-libnotify+.c 

pidgin_libnotify__la_LIBADD = \
	$(LIBNOTIFY_LIBS)

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = \
	-DVERSION=\"$(VERSION)\" \
	-DDATADIR=\"$(PIDGIN_DATADIR)\" \
	-DLOCALEDIR=\"$(PIDGIN_DATADIR)/locale\" \
	$(PIDGIN_CFLAGS) \
	$(LIBNOTIFY_CFLAGS) \
	$(DEBUG_CFLAGS)

