/*
 * purple-libnotify+ - Provide libnotify interface to Pidgin and Finch
 * Copyright © 2010-2012 Quentin "Sardem FF7" Glidic
 *
 * This file is part of purple-libnotify+.
 *
 * purple-libnotify+ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * purple-libnotify+ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with purple-libnotify+.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "purple-libnotify+-common.h"

#include <libnotify/notify.h>
#include "libnotify-compat.h"

#include <purple-events.h>

#include "purple-libnotify+-utils.h"

static GdkPixbuf *
_notify_plus_get_buddy_icon_pixbuf(PurpleBuddy *buddy)
{
	PurpleBuddyIcon *buddy_icon = purple_buddy_get_icon(buddy);
	if ( buddy_icon == NULL )
		return NULL;

	size_t len;
	const guchar *data = purple_buddy_icon_get_data(buddy_icon, &len);

	GdkPixbufLoader *loader = gdk_pixbuf_loader_new();
	gdk_pixbuf_loader_write(loader, data, len, NULL);
	gdk_pixbuf_loader_close(loader, NULL);

	GdkPixbuf *icon = gdk_pixbuf_loader_get_pixbuf(loader);

	if ( icon != NULL )
		g_object_ref(icon);

	g_object_unref(loader);

	return icon;
}

static GdkPixbuf *
_notify_plus_get_notificitaion_pixbuf(PurpleBuddy *buddy, const gchar *protocol_icon_filename)
{
	GError *error = NULL;

	GdkPixbuf *icon = _notify_plus_get_buddy_icon_pixbuf(buddy);
	if ( icon == NULL )
		return NULL;

	if ( ! notify_plus_data.overlay_icon )
		return icon;

	gdouble scale = (gdouble)purple_prefs_get_int("/plugins/core/libnotify+/overlay-scale") / 100.;
	if ( scale <= 0.0 )
		return icon;

	if ( ( protocol_icon_filename == NULL ) || ( ! g_file_test(protocol_icon_filename, G_FILE_TEST_IS_REGULAR) ) )
		return icon;

	GdkPixbuf *protocol_icon = gdk_pixbuf_new_from_file(protocol_icon_filename, &error);
	if ( protocol_icon == NULL )
	{
		g_warning("Couldn’t load protocol icon file: %s", error->message);
		g_clear_error(&error);
		return icon;
	}

	gint icon_width, icon_height;
	gint overlay_icon_width, overlay_icon_height;
	gint x, y;

	icon_width = gdk_pixbuf_get_width(icon);
	icon_height = gdk_pixbuf_get_height(icon);


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

	return icon;
}

static void
notification_closed_cb(NotifyNotification *notification, gpointer user_data)
{
	purple_events_handler_remove_event(notify_plus->extra, user_data, notification);
	g_object_unref(G_OBJECT(notification));
}

static NotifyNotification *
_notify_plus_send_notification_internal(
	NotifyNotification *notification,
	const gchar *title,
	const gchar *body,
	const gchar *protocol_icon_uri,
	const gchar *protocol_icon_filename,
	PurpleBuddy *buddy,
	gpointer attach
	)
{
	GError *error = NULL;

	if ( notification != NULL )
		notify_notification_update(notification, title, body, protocol_icon_uri);
	else
	{
		notification = notify_notification_new(title, body, protocol_icon_uri);

		g_signal_connect(notification, "closed", G_CALLBACK(notification_closed_cb), attach);
	}

	notify_notification_set_urgency(notification, NOTIFY_URGENCY_NORMAL);
	gint timeout = purple_prefs_get_int("/plugins/core/libnotify+/expire-timeout");
	if ( timeout < 1 )
		timeout = ( timeout == 0 ) ? NOTIFY_EXPIRES_NEVER : NOTIFY_EXPIRES_DEFAULT;
	notify_notification_set_timeout(notification, timeout);

	if ( notify_plus_data.set_transcient )
	#if GLIB_CHECK_VERSION(2, 26, 0) && HAVE_NOTIFY_06
		notify_notification_set_hint(notification, "transcient", g_variant_new_byte(1));
	#else
		notify_notification_set_hint_byte(notification, "transcient", 1);
	#endif

	GdkPixbuf *icon = _notify_plus_get_notificitaion_pixbuf(buddy, protocol_icon_filename);
	if ( icon != NULL )
	{
		notify_notification_set_image_from_pixbuf(notification, icon);
		g_object_unref(icon);
	}

	if ( ! notify_notification_show(notification, &error) )
	{
		g_warning("Couldn’t send notification: %s", error->message);
		g_clear_error(&error);
	}

	return notification;
}

NotifyNotification *
notify_plus_send_buddy_notification(NotifyNotification *notification, PurpleBuddy *buddy, const gchar *action, const gchar *body, gpointer attach)
{
	if ( ( notification != NULL ) && ( ! notify_plus_data.modify_notification ) )
			return notification;

	gchar *title;
	gchar *es_body = NULL;
	const gchar *protocol_name = NULL;
	gchar *protocol_icon_uri = NULL;
	gchar *protocol_icon_filename = NULL;

	title = g_strdup_printf(action, purple_events_utils_buddy_get_best_name(buddy));

	if ( body != NULL )
		es_body = g_markup_escape_text(body, -1);

	protocol_name = purple_events_utils_buddy_get_protocol(buddy);
	if ( protocol_name != NULL )
	{
		if ( notify_plus_data.use_svg )
			protocol_icon_uri = purple_events_utils_protocol_get_icon_uri(protocol_name, PURPLE_EVENTS_UTILS_ICON_FORMAT_SVG);
		else
		{
			protocol_icon_uri = purple_events_utils_protocol_get_icon_uri(protocol_name, PURPLE_EVENTS_UTILS_ICON_FORMAT_PNG);
			protocol_icon_filename = purple_events_utils_protocol_get_icon_uri(protocol_name, PURPLE_EVENTS_UTILS_ICON_FORMAT_SVG);
		}
	}

	notification = _notify_plus_send_notification_internal(notification, title, es_body, protocol_icon_uri, ( protocol_icon_filename != NULL ) ? (protocol_icon_filename+7) : (protocol_icon_uri+7), buddy, ( attach != NULL ) ? attach : purple_buddy_get_contact(buddy));

	g_free(protocol_icon_filename);
	g_free(protocol_icon_uri);
	g_free(es_body);
	g_free(title);

	return notification;
}
