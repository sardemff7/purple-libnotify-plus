#
# Most of this configure script was ripped from gaim or the
# gaim-encryption configure script, thanks guys ;)
#

AC_INIT([pidgin-libnotify+], [0.1], [nothing])
AC_CANONICAL_SYSTEM
AM_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

AC_PREREQ([2.50])

AC_PATH_PROG(sedpath, sed)
#
# Setup libtool
#
AM_PROG_LIBTOOL
LIBTOOL="$LIBTOOL --silent"

#
# Our header
#

AH_TOP([ /* our header */
#ifndef _KS_CONFIG_H
#define _KS_CONFIG_H
])
AH_BOTTOM([
#endif /* _KS_CONFIG_H */
])

#
# Setup gettext
#
AC_PROG_INTLTOOL
GETTEXT_PACKAGE=pidgin-libnotify+
AC_SUBST(GETTEXT_PACKAGE)
ALL_LINGUAS=""
AM_GLIB_GNU_GETTEXT

#
# Look for the C compiler
#
CFLAGS_save="$CFLAGS"
AC_PROG_CC
CFLAGS="$CFLAGS_save"

#AC_ARG_ENABLE(debug,   [  --enable-debug          compile with debugging support],,enable_debug=no)
#
#if test "x$enable_debug" = "xyes" ; then
#	AC_DEFINE(DEBUG, 1, [Define if debugging is enabled.])
#fi

if test "x$GCC" = "xyes"; then
	CFLAGS="$CFLAGS -Wall -g3"
fi
AC_SUBST(CFLAGS)

#
# Check for libnotify
#

PKG_CHECK_MODULES(LIBNOTIFY, libnotify >= 0.3.2)

AC_SUBST(LIBNOTIFY_CFLAGS)
AC_SUBST(LIBNOTIFY_LIBS)

#
# Check for GTK+
#
PKG_CHECK_MODULES(GTK, gtk+-2.0)

AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

#
# Check for libpurple
#

PKG_CHECK_MODULES(LIBPURPLE, purple >= 2.0.0,
[
	AC_DEFINE(HAVE_LIBPURPLE, 1, [Define if we've found libpurple.])
])

AC_SUBST(LIBPURPLE_CFLAGS)
LIBPURPLE_LIBDIR=`pkg-config --variable=libdir purple`
LIBPURPLE_DATADIR=`pkg-config --variable=datadir purple`
AC_SUBST(LIBPURPLE_LIBDIR)
AC_SUBST(LIBPURPLE_DATADIR)

#
# Check for pidgin
#

PKG_CHECK_MODULES(PIDGIN, pidgin >= 2.0.0,
[
  AC_DEFINE(HAVE_PIDGIN, 1, [Define if we've found pidgin.])
])

AC_SUBST(PIDGIN_CFLAGS)
PIDGIN_LIBDIR=`pkg-config --variable=libdir pidgin`
PIDGIN_DATADIR=`pkg-config --variable=datadir pidgin`
AC_SUBST(PIDGIN_LIBDIR)
AC_SUBST(PIDGIN_DATADIR)

#
# Check if plugins are enabled
#
want_plugins=yes
AM_CONDITIONAL(PLUGINS, test "$want_plugins" = "yes")

#
# Version stuff
#
AC_CONFIG_COMMANDS_PRE([
	if test -e VERSION; then
		cp -p VERSION VERSION.ac-save
	fi
])

AC_CONFIG_COMMANDS_POST([
	cmp VERSION VERSION.ac-save || touch -r VERSION.ac-save VERSION
	rm -f VERSION.ac-save
])

#
# Finish up
#
AC_OUTPUT([Makefile
		   VERSION
		   po/Makefile.in
		   src/Makefile
		  ])

echo;
echo Configuration complete
echo;
echo Debugging enabled..............: $enable_debug
#echo libpurple API..................: $LIBPURPLE_CFLAGS
#echo pidgin API.....................: $PIDGIN_CFLAGS
echo;
echo Type make to compile
echo Thank you for using pidgin-libnotify+
echo;
