/*
 * Pidgin-Libnotify+ - Provide libnotify interface to Pidgin
 * Copyright (C) 2010 Sardem FF7
 * 
 * This file is part of Pidgin-Libnotify+.
 * 
 * Pidgin-Libnotify+ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Pidgin-Libnotify+ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Pidgin-Libnotify+.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __PIDGIN_LIBNOTIFY_PLUS_COMMON_H__
#define __PIDGIN_LIBNOTIFY_PLUS_COMMON_H__

#include <purple.h>

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#ifdef ENABLE_NLS
	#include <locale.h>
	#include <libintl.h>
	#define _(x) dgettext(PACKAGE, x)
	#ifdef dgettext_noop
		#define N_(x) dgettext_noop(PACKAGE, x)
	#else
		#define N_(x) (x)
	#endif
#else
	#include <locale.h>
	#define _(x) (x)
	#define ngettext(Singular, Plural, Number) ((Number == 1) ? (Singular) : (Plural))
	#define N_(x) (x)
#endif

static struct
{
	GHashTable *notifications;
	GList *just_signed_on_accounts;
} notify_plus_data;

#endif /* __PIDGIN_LIBNOTIFY_PLUS_COMMON_H__ */
