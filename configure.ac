AC_PREREQ([2.65])
AC_INIT([purple-libnotify+], [1.2.2], [], [])

AC_CONFIG_SRCDIR([src/])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([.aux])

AC_SUBST([PACKAGE_NAME], ["$PACKAGE_NAME"])
AC_SUBST([PACKAGE_VERSION], ["$PACKAGE_VERSION"])

AM_INIT_AUTOMAKE([1.10 dist-xz no-dist-gzip foreign])
AM_MAINTAINER_MODE

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_PROG_CC
AC_PROG_LIBTOOL

GETTEXT_PACKAGE=$PACKAGE_NAME
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Gettext Package])
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.35.0])

AC_SUBST([localedir], ["$localedir"])

AC_PROG_INSTALL

#
# Check for libnotify
#
PKG_CHECK_MODULES(LIBNOTIFY, libnotify >= 0.3.2)
# Check for libnotify >= 0.7
PKG_CHECK_EXISTS([libnotify >= 0.7],
	AC_DEFINE(HAVE_LIBNOTIFY_07, 1, [Define if you have libnotify 0.7 or later])
)
AC_SUBST(LIBNOTIFY_CFLAGS)
AC_SUBST(LIBNOTIFY_LIBS)

#
# Check for gdk-pixbuf
#
PKG_CHECK_MODULES(GDK_PIXBUF, gdk-pixbuf-2.0 >= 0.26)
AC_SUBST(GDK_PIXBUF_CFLAGS)
AC_SUBST(GDK_PIXBUF_LIBS)

#
# Check for pidgin
#
PKG_CHECK_MODULES(PURPLE, pidgin >= 2.6.0)
AC_SUBST(PURPLE_CFLAGS)
AC_SUBST(PURPLE_LIBS)
PURPLE_LIBDIR=`pkg-config --variable=libdir purple`
AC_SUBST(PURPLE_LIBDIR)
PURPLE_DATADIR=`pkg-config --variable=datadir purple`
AC_SUBST(PURPLE_DATADIR)


AC_MSG_CHECKING([whether to activate debug])
AC_ARG_ENABLE([debug],
	AS_HELP_STRING([--enable-debug], [Activate debug]),
	[debug=$enableval
	AC_MSG_RESULT([$enableval])],
	[debug=no
	AC_MSG_RESULT([no])])
if test "x$debug" = "xyes"; then
	DEBUG_CFLAGS="-g -Wall -Werror"
	AC_SUBST(DEBUG_CFLAGS)
fi

AC_CONFIG_FILES([
	Makefile
	po/Makefile.in
])
AC_OUTPUT
