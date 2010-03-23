gddir = $(PIDGIN_LIBDIR)/pidgin

pidgin_libnotify__la_LDFLAGS = -module -avoid-version

if PLUGINS

gd_LTLIBRARIES = pidgin-libnotify+.la

pidgin_libnotify__la_SOURCES = \
	pidgin-libnotify+.c 

pidgin_libnotify__la_LIBADD = $(LIBNOTIFY_LIBS) $(DBUS_LIBS) $(GTK_LIBS)

endif

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = \
	-DVERSION=\"$(VERSION)\" \
	-DDATADIR=\"$(PIDGIN_DATADIR)\" \
	-DLOCALEDIR=\"$(PIDGIN_DATADIR)/locale\" \
	$(DEBUG_CFLAGS) \
	$(GTK_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(PIDGIN_CFLAGS) \
	$(LIBPURPLE_CFLAGS) \
	$(LIBNOTIFY_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(GTK_CFLAGS)

