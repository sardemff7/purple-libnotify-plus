AC_PREREQ([2.65])
AC_INIT([pidgin-libnotify+], [0.9], [nothing])

AC_CONFIG_SRCDIR([src/])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC

if test "x$GCC" = "xyes"; then
	CFLAGS="$CFLAGS -Wall -g3"
fi
AC_SUBST(CFLAGS)

dnl ================================================================
dnl Debugging
dnl ================================================================
AC_ARG_ENABLE(
	debug,
	AC_HELP_STRING([--enable-debug], Add debugging messages])
	)
if test x"$enable_debug" = xyes; then
	AC_DEFINE(DEBUG, 1, [Add runtime debugging information (slower)])
fi


AC_PREREQ([2.65])

AC_PROG_LIBTOOL

#
# Setup gettext
#
#AC_PROG_INTLTOOL
#GETTEXT_PACKAGE=pidgin-libnotify+
#AC_SUBST(GETTEXT_PACKAGE)
#ALL_LINGUAS=""
#AM_GLIB_GNU_GETTEXT

# Checks for libraries.

#
# Check for libnotify
#

PKG_CHECK_MODULES(LIBNOTIFY, libnotify >= 0.3.2)

AC_SUBST(LIBNOTIFY_CFLAGS)
AC_SUBST(LIBNOTIFY_LIBS)

#
# Check for GTK+
#
#PKG_CHECK_MODULES(GTK, gtk+-2.0)
#
#AC_SUBST(GTK_CFLAGS)
#AC_SUBST(GTK_LIBS)

#
# Check for libpurple
#

#PKG_CHECK_MODULES(LIBPURPLE, purple >= 2.0.0,
#[
#	AC_DEFINE(HAVE_LIBPURPLE, 1, [Define if we have found libpurple.])
#])
#
#AC_SUBST(LIBPURPLE_CFLAGS)
#LIBPURPLE_LIBDIR=`pkg-config --variable=libdir purple`
#LIBPURPLE_DATADIR=`pkg-config --variable=datadir purple`
#AC_SUBST(LIBPURPLE_LIBDIR)
#AC_SUBST(LIBPURPLE_DATADIR)
#
# Check for pidgin
#

PKG_CHECK_MODULES(PIDGIN, pidgin >= 2.6.0,
[
  AC_DEFINE(HAVE_PIDGIN, 1, [Define if we have found pidgin.])
])

AC_SUBST(PIDGIN_CFLAGS)
PIDGIN_LIBDIR=`pkg-config --variable=libdir pidgin`
PIDGIN_DATADIR=`pkg-config --variable=datadir pidgin`
AC_SUBST(PIDGIN_LIBDIR)
AC_SUBST(PIDGIN_DATADIR)

# Checks for header files.
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.


AC_OUTPUT([Makefile
		   src/Makefile
		  ])
