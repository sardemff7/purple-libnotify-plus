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

#include "pidgin-libnotify+-common.h"
#include "pidgin-libnotify+-utils.h"
#include <libnotify/notify.h>

static gchar *
truncate_string(
	const gchar *str,
	int num_chars
	)
{
	gchar *tr_str;

	if ( g_utf8_strlen(str, num_chars*2+1) > num_chars )
	{
		gchar *str2;

		/* allocate number of bytes and not number of utf-8 chars */
		str2 = g_malloc((num_chars-2) * 2 * sizeof(gchar));

		g_utf8_strncpy(str2, str, num_chars-3);
		tr_str = g_strdup_printf("%s...", str2);
		g_free(str2);
	}
	else
		tr_str = g_strdup(str);

	return tr_str;
}

gchar *
get_best_buddy_name(PurpleBuddy *buddy)
{
	const char *name;
	if ( purple_buddy_get_contact_alias(buddy) )
		name = purple_buddy_get_contact_alias(buddy);
	else if ( purple_buddy_get_alias(buddy) )
		name = purple_buddy_get_alias(buddy);
	else if ( purple_buddy_get_server_alias(buddy) )
		name = purple_buddy_get_server_alias(buddy);
	else
		name = purple_buddy_get_name(buddy);

	gchar *tr_name = truncate_string(name, 25);

	return tr_name;
}

gboolean
is_buddy_notify(PurpleBuddy *buddy)
{
	#ifdef DEBUG
		return TRUE;
	#endif

	PurpleAccount *account = purple_buddy_get_account(buddy);

	if ( g_list_find(notify_plus_data.just_signed_on_accounts, account) )
		return FALSE;

	if ( ( purple_prefs_get_bool("/plugins/core/libnotify+/only-available") )
	&& ( ! purple_status_is_available(purple_account_get_active_status(account)) ) )
		return FALSE;

	const gchar *name = purple_buddy_get_name(buddy);

	if ( ( ! purple_privacy_check(account, name) )
		&& ( purple_prefs_get_bool("/plugins/core/libnotify+/blocked") ) )
		return FALSE;

	PurpleConversation *conv = purple_find_conversation_with_account(PURPLE_CONV_TYPE_IM, name, account);
	if ( ( conv )
		&& (
			( purple_conversation_has_focus(conv) )
			|| ( purple_prefs_get_bool("/plugins/core/libnotify+/new-conv-only") )
		) )
		return FALSE;

	PurpleBlistNode *contact = &(purple_buddy_get_contact(buddy)->node);
	gint deactivate = purple_blist_node_get_int(contact, PACKAGE_NAME"/deactivate");
	if ( deactivate == 0 )
	{
		PurpleBlistNode *group = &(purple_buddy_get_group(buddy)->node);
		deactivate = purple_blist_node_get_int(group, PACKAGE_NAME"/deactivate");
	}
	return ( deactivate != 1 );
}

static GdkPixbuf *
pixbuf_from_buddy_icon(PurpleBuddyIcon *buddy_icon)
{
	size_t len;
	const guchar *data = purple_buddy_icon_get_data(buddy_icon, &len);

	GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
	gdk_pixbuf_loader_set_size(loader, 48, 48);
	gdk_pixbuf_loader_write(loader, data, len, NULL);
	gdk_pixbuf_loader_close(loader, NULL);

	GdkPixbuf *icon = gdk_pixbuf_loader_get_pixbuf(loader);

	if ( icon )
		g_object_ref(icon);

	g_object_unref(loader);

	return icon;
}

static gboolean
notification_closed_cb(NotifyNotification *notification)
{
	PurpleContact *contact = (PurpleContact *)g_object_get_data(G_OBJECT(notification), "contact");
	if ( contact )
	{
		GList *list = g_hash_table_lookup(notify_plus_data.notifications, contact);
		list = g_list_remove(list, notification);
		if ( list == NULL )
			g_hash_table_remove(notify_plus_data.notifications, contact);
	}

	g_hash_table_unref(notify_plus_data.notifications);
	g_object_unref(G_OBJECT(notification));

	return FALSE;
}

void
send_notification(
	const gchar *title,
	const gchar *body,
	PurpleBuddy *buddy
	)
{
	NotifyNotification *notification = NULL;


	gchar *es_body;
	if ( body )
	{
		gchar *tr_body = truncate_string(body, 60);
		es_body = g_markup_escape_text(tr_body, -1);
		g_free(tr_body);
	}
	else
		es_body = NULL;

	PurpleContact *contact = purple_buddy_get_contact(buddy);
	GList *list = g_hash_table_lookup(notify_plus_data.notifications, contact);
	if ( ( ! purple_prefs_get_bool("/plugins/core/libnotify+/stack-notifications") )
	&& ( list ) )
	{
		#ifdef MODIFY_NOTIFY
		notify_notification_update(list->data, title, es_body, NULL);
		notify_notification_show(list->data, NULL);
		#endif /* MODIFY_NOTIFY */

		g_free(es_body);
		return;
	}

	notification = notify_notification_new(title, es_body, NULL
	#ifndef HAVE_LIBNOTIFY_07
		, NULL
	#endif
	);

	g_free(es_body);


	notify_notification_set_urgency(notification, NOTIFY_URGENCY_NORMAL);
	gint timeout = purple_prefs_get_int("/plugins/core/libnotify+/expire-timeout");
	if ( timeout < 1 )
		timeout = ( timeout == 0 ) ? NOTIFY_EXPIRES_NEVER : NOTIFY_EXPIRES_DEFAULT;
	notify_notification_set_timeout(notification, timeout);

	PurpleBuddyIcon *buddy_icon = NULL;
	GdkPixbuf *icon = NULL;
	if ( buddy_icon )
		icon = pixbuf_from_buddy_icon(buddy_icon);

	if ( icon )
	{
		#ifndef HAVE_LIBNOTIFY_07
		notify_notification_set_icon_from_pixbuf(notification, icon);
		#else
		notify_notification_set_image_from_pixbuf(notification, icon);
		#endif
		g_object_unref(icon);
	}


	list = g_list_prepend(list, notification);
	g_hash_table_insert(notify_plus_data.notifications, contact, list);
	g_hash_table_ref(notify_plus_data.notifications);
	g_object_set_data(G_OBJECT(notification), "contact", contact);
	g_signal_connect(notification, "closed", G_CALLBACK(notification_closed_cb), NULL);

	#ifdef DEBUG
		if ( ! notify_notification_show(notification, NULL) )
		{
				gchar *err_body;
				err_body = g_strdup_printf("%s\n%s", title, body);
				purple_notify_message(
					notify_plus, PURPLE_NOTIFY_MSG_ERROR,
					"Notification send error", "Failed to send a notification", err_body, NULL, NULL
					);
				g_free(err_body);
		}
	#else
		notify_notification_show(notification, NULL);
	#endif
}
