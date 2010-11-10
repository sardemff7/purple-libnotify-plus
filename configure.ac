AC_PREREQ(2.65)
AC_INIT([pidgin-libnotify+], [0.9], [])

AC_CONFIG_SRCDIR([src/])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([.aux])

AM_INIT_AUTOMAKE
AC_LANG([C])

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for header files.
AC_CHECK_HEADERS([string.h])


m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_MSG_CHECKING([whether to activate debug])
AC_ARG_ENABLE([debug],
	AS_HELP_STRING([--enable-debug], [Activate debug]),
	[debug=$enableval
	AC_MSG_RESULT([$enableval])],
	[debug=no
	AC_MSG_RESULT([no])])

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
# Check for pidgin
#

PKG_CHECK_MODULES(PIDGIN, pidgin >= 2.6.0)

AC_SUBST(PIDGIN_CFLAGS)
PIDGIN_LIBDIR=`pkg-config --variable=libdir pidgin`
PIDGIN_DATADIR=`pkg-config --variable=datadir pidgin`
AC_SUBST(PIDGIN_LIBDIR)
AC_SUBST(PIDGIN_DATADIR)


if test "x$debug" = "xyes"; then
	DEBUG_CFLAGS="-g -Wall -Werror"
	AC_SUBST(DEBUG_CFLAGS)
fi

AC_CONFIG_FILES([
	Makefile
	src/Makefile
])
AC_OUTPUT
