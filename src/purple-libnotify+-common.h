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

#ifndef __PIDGIN_LIBNOTIFY_PLUS_COMMON_H__
#define __PIDGIN_LIBNOTIFY_PLUS_COMMON_H__

#include <glib.h>
#include <glib/gi18n-lib.h>
#include <purple.h>

typedef struct {
	guint handle;
	PurpleAccount *account;
} JustSignedOnAccount;

struct
{
	GHashTable *notifications;
	GList *just_signed_on_accounts;
	gboolean modify_notification;
	gboolean set_transcient;
	gboolean truncate;
} notify_plus_data;

#endif /* __PIDGIN_LIBNOTIFY_PLUS_COMMON_H__ */
