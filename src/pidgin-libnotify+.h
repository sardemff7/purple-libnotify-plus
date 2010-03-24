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

#ifndef PIDGIN_LIBNOTIFY_PLUS_H
#define PIDGIN_LIBNOTIFY_PLUS_H

#include <purple.h>
/*
#include <pidgin.h>
#include <plugin.h>
#include <debug.h>
#include <util.h>
#include <privacy.h>
*/

#include <gtkutils.h>



#include <string.h>

#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#ifdef ENABLE_NLS
	#include <locale.h>
	#include <libintl.h>
	#define _(x) dgettext(PACKAGE, x)
	#ifdef dgettext_noop
		#define N_(String) dgettext_noop(PACKAGE, String)
	#else
		#define N_(String) (String)
	#endif
#else
	#include <locale.h>
	#define N_(String) (String)
	#define _(x) (x)
	#define ngettext(Singular, Plural, Number) ((Number == 1) ? (Singular) : (Plural))
#endif

#include <libnotify/notify.h>

#define PLUGIN_ID "pidgin-libnotify+"

static void
notify_plus_buddy_signed_on_cb(
	PurpleBuddy *buddy,
	gpointer data
	);

static void
notify_plus_buddy_signed_off_cb(
	PurpleBuddy *buddy,
	gpointer data
	);

static void
notify_plus_new_im_msg_cb(
	PurpleAccount *account,
	const gchar *sender,
	const gchar *message,
	int flags,
	gpointer data
	);

static void
notify_plus_new_chat_msg_cb(
	PurpleAccount *account,
	const gchar *sender,
	const gchar *message,
	PurpleConversation *conv,
	gpointer data
	);

static void
send_notification(
	const gchar *title,
	const gchar *body,
	PurpleBuddy *buddy
	);

static gboolean
plugin_load(PurplePlugin *plugin);

static gboolean
plugin_unload(PurplePlugin *plugin);


static void
init_plugin(PurplePlugin *plugin);

#endif /* PIDGIN_LIBNOTIFY_PLUS_H */
