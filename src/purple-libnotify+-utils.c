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

#include "purple-libnotify+-common.h"

#include <libnotify/notify.h>
#if ! NOTIFY_CHECK_VERSION(0,7,0)
#include "libnotify-compat.h"
#endif

#include "purple-libnotify+-utils.h"

static gchar *
truncate_string(
	const gchar *str,
	int num_chars
	)
{
	gchar *tr_str;

	tr_str = g_strdup(str);

	if ( g_utf8_strlen(str, -1) > num_chars )
	{
		gchar *tmp;

		/* keep num_chars characters */
		tmp = g_utf8_offset_to_pointer(tr_str, num_chars+1);
		*tmp = 0;

		/* add ellipsis */
		tmp = tr_str;
		tr_str = g_strconcat(tmp, "…", NULL);
		g_free(tmp);
	}

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
	#if DEBUG
		return TRUE;
	#endif

	GList *list;
	PurpleAccount *account = purple_buddy_get_account(buddy);

	for ( list = g_list_first(notify_plus_data.just_signed_on_accounts) ; list != NULL ; list = g_list_next(list) )
	{
		JustSignedOnAccount *just_signed_on_account = (JustSignedOnAccount *)list->data;
		if ( account == just_signed_on_account->account )
			return FALSE;
	}

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
		#if MODIFY_NOTIFY
		notify_notification_update(list->data, title, es_body, NULL);
		notify_notification_show(list->data, NULL);
		#endif /* MODIFY_NOTIFY */

		g_free(es_body);
		return;
	}

	notification = notify_notification_new(title, es_body, NULL);

	g_free(es_body);


	notify_notification_set_urgency(notification, NOTIFY_URGENCY_NORMAL);
	gint timeout = purple_prefs_get_int("/plugins/core/libnotify+/expire-timeout");
	if ( timeout < 1 )
		timeout = ( timeout == 0 ) ? NOTIFY_EXPIRES_NEVER : NOTIFY_EXPIRES_DEFAULT;
	notify_notification_set_timeout(notification, timeout);

	{
		PurpleBuddyIcon *buddy_icon = NULL;
		GdkPixbuf *icon = NULL;
		GdkPixbuf *protocol_icon = NULL;
		PurplePluginProtocolInfo *info;
		gchar *protoname = NULL;
		gchar *filename = NULL;

		buddy_icon = purple_buddy_get_icon(buddy);
		if ( buddy_icon )
			icon = pixbuf_from_buddy_icon(buddy_icon);


		info = PURPLE_PLUGIN_PROTOCOL_INFO(purple_find_prpl(purple_account_get_protocol_id(buddy->account)));
		if ( info->list_icon != NULL )
			protoname = info->list_icon(buddy->account, NULL);

		if ( protoname != NULL )
		{
			gchar *tmp;
			tmp = g_strconcat(protoname, ".svg", NULL);
			filename = g_build_filename(PURPLE_DATADIR, "pixmaps", "pidgin", "protocols", "scalable", tmp, NULL);
			g_free(tmp);
		}

		if ( ( filename != NULL ) && ( g_file_test(filename, G_FILE_TEST_IS_REGULAR) ) )
		{
			GError *error = NULL;

			protocol_icon = gdk_pixbuf_new_from_file(filename, &error);

			if ( protocol_icon == NULL )
			{
				g_warning("Couldn’t load protocol icon file: %s", error->message);
				g_clear_error(&error);
			}
		}

		if ( icon != NULL )
		{
			if ( protocol_icon != NULL )
			{
				gint icon_width, icon_height;
				gint overlay_icon_width, overlay_icon_height;
				gint x, y;
				gdouble scale;

				icon_width = gdk_pixbuf_get_width(icon);
				icon_height = gdk_pixbuf_get_height(icon);

				scale = (gdouble)purple_prefs_get_int("/plugins/core/libnotify+/overlay-scale") / 100.;

				overlay_icon_width = scale * (gdouble)icon_width;
				overlay_icon_height = scale * (gdouble)icon_height;

				x = icon_width - overlay_icon_width;
				y = icon_height - overlay_icon_height;

				scale = (gdouble)overlay_icon_width / (gdouble)gdk_pixbuf_get_width(protocol_icon);

				gdk_pixbuf_composite(protocol_icon, icon,
									 x, y,
									 overlay_icon_width, overlay_icon_height,
									 x, y,
									 scale, scale,
									 GDK_INTERP_BILINEAR, 255);

				g_object_unref(protocol_icon);
			}

			notify_notification_set_image_from_pixbuf(notification, icon);
			g_object_unref(icon);
		}
		else if ( protocol_icon != NULL )
		{
			notify_notification_set_image_from_pixbuf(notification, protocol_icon);
			g_object_unref(protocol_icon);
		}
	}

	list = g_list_prepend(list, notification);
	g_hash_table_insert(notify_plus_data.notifications, contact, list);
	g_hash_table_ref(notify_plus_data.notifications);
	g_object_set_data(G_OBJECT(notification), "contact", contact);
	g_signal_connect(notification, "closed", G_CALLBACK(notification_closed_cb), NULL);

	#if DEBUG
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
