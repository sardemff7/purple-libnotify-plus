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

#include <purple-events.h>

#include "purple-libnotify+-utils.h"
#include "purple-libnotify+-frames.h"

PurplePlugin *notify_plus = NULL;

static gpointer
_purple_notify_plus_signed_on(PurplePlugin *plugin, gpointer notification, PurpleBuddy *buddy)
{
	return notify_plus_send_buddy_notification(notification, buddy,_("%s signed on"), NULL, NULL);
}

static gpointer
_purple_notify_plus_signed_off(PurplePlugin *plugin, gpointer notification, PurpleBuddy *buddy)
{
	return notify_plus_send_buddy_notification(notification, buddy,_("%s signed off"), NULL, NULL);
}

static gpointer
_purple_notify_plus_away(PurplePlugin *plugin, gpointer notification, PurpleBuddy *buddy, const gchar *message)
{
	return notify_plus_send_buddy_notification(notification, buddy, _("%s went away"), message, NULL);
}

static gpointer
_purple_notify_plus_back(PurplePlugin *plugin, gpointer notification, PurpleBuddy *buddy, const gchar *message)
{
	return notify_plus_send_buddy_notification(notification, buddy, _("%s came back"), message, NULL);
}

static gpointer
_purple_notify_plus_status(PurplePlugin *plugin, gpointer notification, PurpleBuddy *buddy, const gchar *message)
{
	return notify_plus_send_buddy_notification(notification, buddy, ( message != NULL ) ? _("%s changed status message") : _("%s removed status message"), message, NULL);
}

static gpointer
_purple_notify_plus_special(PurplePlugin *plugin, gpointer notification, PurpleBuddy *buddy, PurpleEventsEventSpecialType type, ...)
{
	return NULL;
}

static gpointer
_purple_notify_plus_idle(PurplePlugin *plugin, gpointer notification, PurpleBuddy *buddy)
{
	return notify_plus_send_buddy_notification(notification, buddy, _("%s went idle"), NULL, NULL);
}

static gpointer
_purple_notify_plus_idle_back(PurplePlugin *plugin, gpointer notification, PurpleBuddy *buddy)
{
	return notify_plus_send_buddy_notification(notification, buddy, _("%s came back idle"), NULL, NULL);
}

static gpointer
_purple_notify_plus_im_message(PurplePlugin *plugin, gpointer notification, PurpleBuddy *buddy, const gchar *message)
{
	gchar *body;
	gchar *tmp;

	tmp = purple_markup_strip_html(message);
	body = g_strdup_printf(_("“%s”"), tmp);
	g_free(tmp);

	notification = notify_plus_send_buddy_notification(notification, buddy, "%s", body, NULL);

	g_free(body);

	return notification;
}

static gpointer
_purple_notify_plus_im_action(PurplePlugin *plugin, gpointer notification, PurpleBuddy *buddy, const gchar *message)
{
	gchar *body;

	body = purple_markup_strip_html(message);

	notification = notify_plus_send_buddy_notification(notification, buddy, "%s", body, NULL);

	g_free(body);

	return notification;
}

static gpointer
_purple_notify_plus_chat_message(PurplePlugin *plugin, gpointer notification, PurpleConversation *conv, PurpleBuddy *buddy, const gchar *message)
{
	gchar *body;
	gchar *tmp;

	tmp = purple_markup_strip_html(message);
	body = g_strdup_printf(_("“%s”"), tmp);
	g_free(tmp);

	notification = notify_plus_send_buddy_notification(notification, buddy, "%s", body, conv);

	g_free(body);

	return notification;
}

static gpointer
_purple_notify_plus_chat_action(PurplePlugin *plugin, gpointer notification, PurpleConversation *conv, PurpleBuddy *buddy, const gchar *message)
{
	gchar *body;

	body = purple_markup_strip_html(message);

	notification = notify_plus_send_buddy_notification(notification, buddy, "%s", body, conv);

	g_free(body);

	return notification;
}

static void
_purple_notify_plus_email_action_callback(NotifyNotification *notification, char *action, gpointer user_data)
{
	const gchar *url = user_data;
	purple_notify_uri(notify_plus, url);
}

static void
_purple_notify_plus_email(PurplePlugin *plugin, const gchar *subject, const gchar *from, const gchar *to, const gchar *url)
{
	gchar *title;

	title = g_strdup_printf(_("New e-mail from %s"), from);

	notify_plus_send_notification_with_actions(title, subject, "mail-unread", NULL, NULL, "default", _("Show"), _purple_notify_plus_email_action_callback, url, NULL, NULL);

	g_free(title);
}

static void
_notify_plus_end_event(PurplePlugin *plugin, gpointer event)
{
	GError *error = NULL;
	if ( ! notify_notification_close(event, &error) )
	{
		g_warning("Couldn’t close notification: %s", error->message);
		g_clear_error(&error);
	}
}

static void
notify_plus_adapt_to_server_capabilities()
{
	GList *capabilities;
	GList *capability;

	notify_plus_data.modify_notification = TRUE;
	notify_plus_data.use_svg = FALSE;
	notify_plus_data.overlay_icon = TRUE;
	notify_plus_data.set_transcient = FALSE;
	notify_plus_data.actions = FALSE;

	capabilities = notify_get_server_caps();
	for ( capability = capabilities ; capability != NULL ; capability = g_list_next(capability) )
	{
		gchar *cap_name = capability->data;

		if ( g_strcmp0(cap_name, "persistence") == 0 )
			notify_plus_data.set_transcient = TRUE;
		else if ( g_strcmp0(cap_name, "image/svg+xml") == 0 )
			notify_plus_data.use_svg = TRUE;
		else if ( g_strcmp0(cap_name, "x-eventd-overlay-icon") == 0 )
			notify_plus_data.overlay_icon = FALSE;
		else if ( g_strcmp0(cap_name, "x-canonical-append") == 0 )
			notify_plus_data.modify_notification = FALSE;
		else if ( g_strcmp0(cap_name, "actions") == 0 )
			notify_plus_data.actions = TRUE;

		g_free(cap_name);
	}

	g_list_free(capabilities);
}

static gboolean
plugin_load(PurplePlugin *plugin)
{
	if ( ( ! notify_is_initted() ) && ( ! notify_init("libpurple") ) )
	{
		purple_debug_error(PACKAGE_NAME, "libnotify not running!\n");
		return FALSE;
	}
	notify_plus_adapt_to_server_capabilities();

	purple_events_connect_handler(plugin->extra);

	return TRUE;
}

static gboolean
plugin_unload(PurplePlugin *plugin)
{
	purple_events_disconnect_handler(plugin->extra);

	notify_uninit();

	return TRUE;
}

static void
_purple_notify_plus_destroy(PurplePlugin *plugin)
{
	purple_events_handler_free(plugin->extra);
}


static PurplePluginUiInfo prefs_info = {
    .get_plugin_pref_frame = notify_plus_pref_frame
};

static PurplePluginInfo info = {
    .magic          = PURPLE_PLUGIN_MAGIC,
    .major_version  = PURPLE_MAJOR_VERSION,
    .minor_version  = PURPLE_MINOR_VERSION,
    .type           = PURPLE_PLUGIN_STANDARD,
    .ui_requirement = 0,
    .flags          = 0,
    .dependencies   = NULL,
    .priority       = PURPLE_PRIORITY_DEFAULT,

    .id             = "core-sardemff7-" PACKAGE_NAME,
    .name           = "Libnotify+",
    .version        = PACKAGE_VERSION,
    .summary        = NULL,
    .description    = NULL,
    .author         = "Quentin \"Sardem FF7\" Glidic <sardemff7+pidgin@sardemff7.net>",
    .homepage       = "http://sardemff7.github.com/" PACKAGE_TARNAME "/",

    .load           = plugin_load,
    .unload         = plugin_unload,
    .destroy        = _purple_notify_plus_destroy,

    .ui_info        = NULL,
    .extra_info     = NULL,
    .prefs_info     = &prefs_info,
    .actions        = NULL
};

static void
init_plugin(PurplePlugin *plugin)
{
	notify_plus = plugin;

	#if ENABLE_NLS
		bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
		bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	#endif

	info.summary = _("Displays popups via libnotify.");
	info.description = _("Displays popups via libnotify.");
	info.dependencies = g_list_prepend(info.dependencies, (gpointer) purple_events_get_plugin_id());

	gint timeout = -1;

	if ( purple_prefs_exists("/plugins/gtk/libnotify+") )
	{
		timeout = purple_prefs_get_int("/plugins/gtk/libnotify+/expire-timeout");
		purple_prefs_remove("/plugins/gtk/libnotify+");
	}

	if ( purple_prefs_exists("/plugins/core/libnotify+/new-msg") )
	{
		purple_prefs_remove("/plugins/core/libnotify+/new-msg");
		purple_prefs_remove("/plugins/core/libnotify+/signed-on");
		purple_prefs_remove("/plugins/core/libnotify+/signed-off");
		purple_prefs_remove("/plugins/core/libnotify+/away");
		purple_prefs_remove("/plugins/core/libnotify+/idle");
		purple_prefs_remove("/plugins/core/libnotify+/back");
		purple_prefs_remove("/plugins/core/libnotify+/status-message");
		purple_prefs_remove("/plugins/core/libnotify+/blocked");
		purple_prefs_remove("/plugins/core/libnotify+/new-conv-only");
		purple_prefs_remove("/plugins/core/libnotify+/only-available");
		purple_prefs_remove("/plugins/core/libnotify+/stack-notifications");
	}

	purple_prefs_add_none("/plugins/core/libnotify+");
	purple_prefs_add_int("/plugins/core/libnotify+/expire-timeout", timeout);
	purple_prefs_add_int("/plugins/core/libnotify+/overlay-scale", 50);
	purple_prefs_add_bool("/plugins/core/libnotify+/no-transcient", FALSE);

	PurpleEventsHandler *handler;

	handler = purple_events_handler_new(plugin);
	plugin->extra = handler;

	purple_events_handler_add_signed_on_callback(handler, _purple_notify_plus_signed_on);
	purple_events_handler_add_signed_off_callback(handler, _purple_notify_plus_signed_off);

	purple_events_handler_add_away_callback(handler, _purple_notify_plus_away);
	purple_events_handler_add_back_callback(handler, _purple_notify_plus_back);

	purple_events_handler_add_status_callback(handler, _purple_notify_plus_status);
	purple_events_handler_add_special_callback(handler, _purple_notify_plus_special);

	purple_events_handler_add_idle_callback(handler, _purple_notify_plus_idle);
	purple_events_handler_add_idle_back_callback(handler, _purple_notify_plus_idle_back);

	purple_events_handler_add_im_message_callback(handler, _purple_notify_plus_im_message);
	purple_events_handler_add_im_action_callback(handler, _purple_notify_plus_im_action);

	purple_events_handler_add_chat_message_callback(handler, _purple_notify_plus_chat_message);
	purple_events_handler_add_chat_action_callback(handler, _purple_notify_plus_chat_action);

	purple_events_handler_add_email_callback(handler, _purple_notify_plus_email);

	purple_events_handler_add_end_event_callback(handler, _notify_plus_end_event);
}

PURPLE_INIT_PLUGIN(notify_plus, init_plugin, info)
