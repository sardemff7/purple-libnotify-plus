AM_DISTCHECK_CONFIGURE_FLAGS = \
	--disable-debug \
	--with-purple-plugindir="$$dc_install_base/$(plugindir)"

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
AM_MAKEFLAGS = --no-print-directory

SUBDIRS = po

AM_CFLAGS += \
	-DLOCALEDIR=\"$(localedir)\" \
	-fcommon \
	--include config.h

plugin_LTLIBRARIES = purple-libnotify+.la

purple_libnotify__la_SOURCES = \
	src/purple-libnotify+-frames.h \
	src/purple-libnotify+-utils.h \
	src/purple-libnotify+-common.h \
	src/purple-libnotify+-frames.c \
	src/purple-libnotify+-utils.c \
	src/purple-libnotify+.c

purple_libnotify__la_CFLAGS = \
	$(AM_CFLAGS) \
	$(PURPLE_EVENTS_CFLAGS) \
	$(PURPLE_CFLAGS) \
	$(GDK_PIXBUF_CFLAGS) \
	$(NOTIFY_CFLAGS) \
	$(GLIB_CFLAGS)

purple_libnotify__la_LDFLAGS = \
	-no-undefined \
	-module -avoid-version \
	-export-symbols-regex 'purple_init_(notify_plus_)?plugin'

purple_libnotify__la_LIBADD = \
	$(PURPLE_EVENTS_LIBS) \
	$(PURPLE_LIBS) \
	$(GDK_PIXBUF_LIBS) \
	$(NOTIFY_LIBS) \
	$(GLIB_LIBS)


#
# Hooks
#

install-data-hook: la-files-install-hook
uninstall-hook: la-files-uninstall-hook

# *.la files cleanup
la-files-install-hook:
	cd $(DESTDIR)$(plugindir) && \
		rm $(plugin_LTLIBRARIES)

# Remove shared object files at uninstall since
# we remove *.la files at install
la-files-uninstall-hook:
	cd $(DESTDIR)$(plugindir) && \
		rm $(plugin_LTLIBRARIES:.la=$(shrext_cmds))
	-rmdir $(DESTDIR)$(plugindir)
