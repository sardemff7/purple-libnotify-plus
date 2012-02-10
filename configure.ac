AC_PREREQ([2.65])
AC_INIT([purple-libnotify+], [1.2.91], [], [purple-libnotify-plus])

AC_CONFIG_SRCDIR([src/])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([.aux])

AM_INIT_AUTOMAKE([1.10 subdir-objects dist-xz no-dist-gzip tar-ustar foreign])

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_PROG_CC
AM_PROG_CC_C_O
AC_DISABLE_STATIC
LT_INIT

GETTEXT_PACKAGE=$PACKAGE_NAME
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Gettext Package])
AC_SUBST(GETTEXT_PACKAGE)
IT_PROG_INTLTOOL([0.35.0])

AC_PROG_INSTALL

AM_CFLAGS="-Wall -Wextra -Wno-unused-parameter"
AM_LDFLAGS=""

#
# Check for glib
#
PKG_CHECK_MODULES(GLIB, glib-2.0)

#
# Check for libnotify
#
PKG_CHECK_MODULES(NOTIFY, libnotify >= 0.5.0)
PKG_CHECK_EXISTS(NOTIFY_06, libnotify >= 0.6.0, [AC_DEFINE([HAVE_NOTIFY_06], [1], [libnotify >= 0.6])], [])

#
# Check for gdk-pixbuf
#
PKG_CHECK_MODULES(GDK_PIXBUF, gdk-pixbuf-2.0 >= 0.26)

#
# Check for pidgin
#
PKG_CHECK_MODULES(PURPLE, pidgin >= 2.6.0)
AC_DEFINE_UNQUOTED([PURPLE_DATADIR], ["`pkg-config --variable=datadir purple`"], [libpurple data directory])
plugindir="`pkg-config --variable=plugindir purple`"
AC_SUBST(plugindir)


AC_ARG_ENABLE(debug, AS_HELP_STRING([--enable-debug], [Enable debug]), enable_debug=$enableval, enable_debug=no)
if test x$enable_debug = xyes; then
    AC_DEFINE([DEBUG], [1], [Activate debug mode])
    AM_CFLAGS="${AM_CFLAGS} -O0 -ggdb3"
fi
AC_SUBST(AM_CFLAGS)
AC_SUBST(AM_LDFLAGS)

AC_CONFIG_FILES([
	Makefile
	po/Makefile.in
])
AC_OUTPUT
