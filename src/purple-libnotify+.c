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

#include <string.h>

#include <libnotify/notify.h>

#include <purple-events.h>

#include "purple-libnotify+-utils.h"
#include "purple-libnotify+-frames.h"

PurplePlugin *notify_plus = NULL;

static void
_purple_notify_plus_signed_on(PurpleBuddy *buddy, PurplePlugin *plugin)
{
	notify_plus_send_buddy_notification(buddy,_("%s signed on"), NULL);
}

static void
_purple_notify_plus_signed_off(PurpleBuddy *buddy, PurplePlugin *plugin)
{
	notify_plus_send_buddy_notification(buddy,_("%s signed off"), NULL);
}

static void
_purple_notify_plus_away(PurpleBuddy *buddy, const gchar *message, PurplePlugin *plugin)
{
	notify_plus_send_buddy_notification(buddy, _("%s went away"), message);
}

static void
_purple_notify_plus_back(PurpleBuddy *buddy, const gchar *message, PurplePlugin *plugin)
{
	notify_plus_send_buddy_notification(buddy, _("%s came back"), message);
}

static void
_purple_notify_plus_status(PurpleBuddy *buddy, const gchar *message, PurplePlugin *plugin)
{
	notify_plus_send_buddy_notification(buddy, ( message != NULL ) ? _("%s changed status message") : _("%s removed status message"), message);
}

static void
_purple_notify_plus_idle(PurpleBuddy *buddy, PurplePlugin *plugin)
{
	notify_plus_send_buddy_notification(buddy, _("%s went idle"), NULL);
}

static void
_purple_notify_plus_idle_back(PurpleBuddy *buddy, PurplePlugin *plugin)
{
	notify_plus_send_buddy_notification(buddy, _("%s came back idle"), NULL);
}

static void
_purple_notify_plus_im_message(PurpleAccount *account, const gchar *sender, const gchar *message, PurpleConversation *conv, PurpleMessageFlags flags, PurplePlugin *plugin)
{
    PurpleBuddy *buddy = purple_find_buddy(account, sender);
	gchar *body = NULL;
	gchar *tmp;

	tmp = purple_markup_strip_html(message);
	body = g_strdup_printf(_("“%s”"), tmp);
	g_free(tmp);

	if ( buddy != NULL )
		notify_plus_send_buddy_notification(buddy, "%s", body);
	else
		notify_plus_send_name_notification(sender, "%s", body, NULL, NULL);

	g_free(body);
}

static void
_purple_notify_plus_im_highlight(PurpleAccount *account, const gchar *sender, const gchar *message, PurpleConversation *conv, PurpleMessageFlags flags, PurplePlugin *plugin)
{
    PurpleBuddy *buddy = purple_find_buddy(account, sender);
	gchar *body = NULL;
	gchar *tmp;

	tmp = purple_markup_strip_html(message);
	body = g_strdup_printf(_("“%s”"), tmp);
	g_free(tmp);

	if ( buddy != NULL )
		notify_plus_send_buddy_notification(buddy, "%s highlighted you", body);
	else
		notify_plus_send_name_notification(sender, "%s highlighted you", body, NULL, NULL);

	g_free(body);
}

static void
_purple_notify_plus_chat_message(PurpleAccount *account, const gchar *sender, const gchar *message, PurpleConversation *conv, PurpleMessageFlags flags, PurplePlugin *plugin)
{
    PurpleBuddy *buddy = purple_find_buddy(account, sender);
	gchar *body = NULL;
	gchar *tmp;

	tmp = purple_markup_strip_html(message);
	body = g_strdup_printf(_("“%s”"), tmp);
	g_free(tmp);

	if ( buddy != NULL )
		notify_plus_send_buddy_notification(buddy, "%s", body);
	else
		notify_plus_send_name_notification(sender, "%s", body, NULL, NULL);

	g_free(body);
}

static void
_purple_notify_plus_chat_highlight(PurpleAccount *account, const gchar *sender, const gchar *message, PurpleConversation *conv, PurpleMessageFlags flags, PurplePlugin *plugin)
{
    PurpleBuddy *buddy = purple_find_buddy(account, sender);
	gchar *body = NULL;
	gchar *tmp;

	tmp = purple_markup_strip_html(message);
	body = g_strdup_printf(_("“%s”"), tmp);
	g_free(tmp);

	if ( buddy != NULL )
		notify_plus_send_buddy_notification(buddy, "%s highlighted you", body);
	else
		notify_plus_send_name_notification(sender, "%s highlighted you", body, NULL, NULL);

	g_free(body);
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

	gpointer handle;

	handle = purple_plugins_find_with_id(purple_events_get_plugin_id());
	g_return_val_if_fail(handle != NULL, FALSE);

	purple_signal_connect(
		handle, "user_presence-online", plugin,
		(PurpleCallback)_purple_notify_plus_signed_on, plugin
	);
	purple_signal_connect(
		handle, "user_presence-offline", plugin,
		(PurpleCallback)_purple_notify_plus_signed_off, plugin
	);
	purple_signal_connect(
		handle, "user_presence-away", plugin,
		(PurpleCallback)_purple_notify_plus_away, plugin
	);
	purple_signal_connect(
		handle, "user_presence-back", plugin,
		(PurpleCallback)_purple_notify_plus_back, plugin
	);
	purple_signal_connect(
		handle, "user_presence-idle", plugin,
		(PurpleCallback)_purple_notify_plus_idle, plugin
	);
	purple_signal_connect(
		handle, "user_presence-idle-back", plugin,
		(PurpleCallback)_purple_notify_plus_idle_back, plugin
	);
	purple_signal_connect(
		handle, "user_presence-message", plugin,
		(PurpleCallback)_purple_notify_plus_status, plugin
	);

	purple_signal_connect(
		handle, "user_im-received", plugin,
		(PurpleCallback)_purple_notify_plus_im_message, plugin
	);
	purple_signal_connect(
		handle, "user_im-highlight", plugin,
		(PurpleCallback)_purple_notify_plus_im_highlight, plugin
	);
	purple_signal_connect(
		handle, "user_chat-received", plugin,
		(PurpleCallback)_purple_notify_plus_chat_message, plugin
	);
	purple_signal_connect(
		handle, "user_chat-highlight", plugin,
		(PurpleCallback)_purple_notify_plus_chat_highlight, plugin
	);

	purple_signal_connect(
		handle, "user_email-arrived", plugin,
		(PurpleCallback)_purple_notify_plus_email, plugin
	);

	return TRUE;
}

static gboolean
plugin_unload(PurplePlugin *plugin)
{
	gpointer handle;

	handle = purple_plugins_find_with_id(purple_events_get_plugin_id());
	g_return_val_if_fail(handle != NULL, FALSE);

	purple_signal_disconnect(
		handle, "user_presence-online", plugin,
		(PurpleCallback)_purple_notify_plus_signed_on
	);
	purple_signal_disconnect(
		handle, "user_presence-offline", plugin,
		(PurpleCallback)_purple_notify_plus_signed_off
	);
	purple_signal_disconnect(
		handle, "user_presence-away", plugin,
		(PurpleCallback)_purple_notify_plus_away
	);
	purple_signal_disconnect(
		handle, "user_presence-back", plugin,
		(PurpleCallback)_purple_notify_plus_back
	);
	purple_signal_disconnect(
		handle, "user_presence-idle", plugin,
		(PurpleCallback)_purple_notify_plus_idle
	);
	purple_signal_disconnect(
		handle, "user_presence-idle-back", plugin,
		(PurpleCallback)_purple_notify_plus_idle_back
	);
	purple_signal_disconnect(
		handle, "user_presence-message", plugin,
		(PurpleCallback)_purple_notify_plus_status
	);

	purple_signal_disconnect(
		handle, "user_im-received", plugin,
		(PurpleCallback)_purple_notify_plus_im_message
	);
	purple_signal_disconnect(
		handle, "user_im-highlight", plugin,
		(PurpleCallback)_purple_notify_plus_im_highlight
	);
	purple_signal_disconnect(
		handle, "user_chat-received", plugin,
		(PurpleCallback)_purple_notify_plus_chat_message
	);
	purple_signal_disconnect(
		handle, "user_chat-highlight", plugin,
		(PurpleCallback)_purple_notify_plus_chat_highlight
	);

	purple_signal_disconnect(
		handle, "user_email-arrived", plugin,
		(PurpleCallback)_purple_notify_plus_email
	);

	notify_uninit();

	return TRUE;
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
    .destroy        = NULL,

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
}

PURPLE_INIT_PLUGIN(notify_plus, init_plugin, info)
