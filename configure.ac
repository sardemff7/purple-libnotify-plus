AC_PREREQ([2.65])
AC_INIT([pidgin-libnotify+], [1.2], [], [])

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
PKG_CHECK_MODULES(LIBNOTIFY_07, [libnotify >= 0.7],
	AC_DEFINE(HAVE_LIBNOTIFY_07, 1, [Define if you have libnotify 0.7 or later])
)
AC_SUBST(LIBNOTIFY_CFLAGS)
AC_SUBST(LIBNOTIFY_LIBS)

#
# Check for pidgin
#
PKG_CHECK_MODULES(PIDGIN, pidgin >= 2.6.0)
AC_SUBST(PIDGIN_CFLAGS)
AC_SUBST(PIDGIN_LIBS)
PIDGIN_LIBDIR=`pkg-config --variable=libdir pidgin`
AC_SUBST(PIDGIN_LIBDIR)
PIDGIN_DATADIR=`pkg-config --variable=datadir pidgin`
AC_SUBST(PIDGIN_DATADIR)


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
