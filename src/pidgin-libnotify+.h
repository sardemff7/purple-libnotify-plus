/*
 * Pidgin-Libnotify+ - Provide libnotify interface to Pidgin
 * Copyright © 2010-2011 Quentin "Sardem FF7" Glidic
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

#ifndef __PIDGIN_LIBNOTIFY_PLUS_H__
#define __PIDGIN_LIBNOTIFY_PLUS_H__


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

static gboolean
plugin_load(PurplePlugin *plugin);

static gboolean
plugin_unload(PurplePlugin *plugin);


static void
init_plugin(PurplePlugin *plugin);

#endif /* __PIDGIN_LIBNOTIFY_PLUS_H__ */
