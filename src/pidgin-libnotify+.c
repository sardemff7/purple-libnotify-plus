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
#include "pidgin-libnotify+-frames.h"
#include "pidgin-libnotify+.h"
#include <libnotify/notify.h>

static PurplePlugin *notify_plus = NULL;

static void
notify_plus_buddy_signed_on_cb(
	PurpleBuddy *buddy,
	gpointer data)
{
	if ( ( ! purple_prefs_get_bool("/plugins/core/libnotify+/signed-on") ) || ( ! is_buddy_notify(buddy) ) )
		return;

	gchar *name = get_best_buddy_name(buddy);
	send_notification(name, _("signed on"), buddy);
	g_free(name);
}

static void
notify_plus_buddy_signed_off_cb(
	PurpleBuddy *buddy,
	gpointer data)
{
	if ( ( ! purple_prefs_get_bool("/plugins/core/libnotify+/signed-off") ) || ( ! is_buddy_notify(buddy) ) )
		return;

	gchar *name = get_best_buddy_name(buddy);
	send_notification(name, _("signed off"), buddy);
	g_free(name);
}

static void
notify_plus_buddy_status_changed_cb(
	PurpleBuddy *buddy,
	PurpleStatus *old_status,
	PurpleStatus *new_status,
	gpointer data)
{
	if ( ( ! is_buddy_notify(buddy) ) || ( ! purple_status_is_exclusive(old_status) ) || ( purple_status_is_exclusive(new_status) ) )
		return;

	gchar *action = NULL;

	gboolean old_avail = purple_status_is_available(old_status);
	gboolean new_avail = purple_status_is_available(new_status);
	const gchar *msg = purple_status_get_attr_string(new_status, "message");
	if ( old_avail && ( ! new_avail ) )
	{
		if ( ! purple_prefs_get_bool("/plugins/core/libnotify+/away") )
			return;
		if ( msg )
			action = g_strdup_printf(_("went away: %s"), msg);
		else
			action = g_strdup(_("went away"));
	}
	else if ( ( ! old_avail ) && new_avail )
	{
		if ( ! purple_prefs_get_bool("/plugins/core/libnotify+/back") )
			return;
		if ( msg )
			action = g_strdup_printf(_("came back: %s"), msg);
		else
			action = g_strdup(_("came back"));
	}
	else if ( old_avail && new_avail )
	{
		if ( ! purple_prefs_get_bool("/plugins/core/libnotify+/status-message") )
			return;
		if ( msg )
			action = g_strdup_printf(_("changed status message to %s"), msg);
		else
			action = g_strdup(_("removed status message"));
	}
	else
		return;

	gchar *name = get_best_buddy_name(buddy);
	send_notification(name, action, buddy);
	g_free(name);
	g_free(action);
}

static void
notify_plus_buddy_idle_changed_cb(
	PurpleBuddy *buddy,
	gboolean oldidle,
	gboolean newidle,
	gpointer data)
{
	if ( ( ! purple_prefs_get_bool("/plugins/core/libnotify+/idle") ) || ( ! is_buddy_notify(buddy) ) )
		return;

	gchar *name = get_best_buddy_name(buddy);
	send_notification(name, ( newidle ) ? ( _("went idle") ) : ( _("came back idle") ), buddy);
	g_free(name);
}

static void
notify_plus_new_im_msg_cb(
	PurpleAccount *account,
	const gchar *sender,
	const gchar *message,
	int flags,
	gpointer data)
{
	PurpleBuddy *buddy = purple_find_buddy(account, sender);
	if ( ( ! purple_prefs_get_bool("/plugins/core/libnotify+/new-msg") ) || ( ! buddy ) || ( ! is_buddy_notify(buddy) ) )
		return;

	gchar *name = get_best_buddy_name(buddy);
	gchar *title = g_strdup_printf(_("%s says"), name);
	g_free(name);

	gchar *body = purple_markup_strip_html(message);

	send_notification(title, body, buddy);

	g_free(title);
	g_free(body);
}

static void
notify_plus_new_chat_msg_cb(
	PurpleAccount *account,
	const gchar *sender,
	const gchar *message,
	PurpleConversation *conv,
	gpointer data)
{
	PurpleBuddy *buddy = purple_find_buddy (account, sender);
	if ( ( ! purple_prefs_get_bool("/plugins/core/libnotify+/new-msg") ) || ( ! buddy ) || ( ! is_buddy_notify(buddy) ) )
		return;

	gchar *body = purple_markup_strip_html(message);
	gchar *name = get_best_buddy_name(buddy);
	gchar *title = g_strdup_printf(_("%s says"), name);

	send_notification(title, body, buddy);

	g_free(title);
	g_free(name);
	g_free(body);
}

static gboolean
event_connection_throttle_cb(gpointer data)
{
	PurpleAccount *account = (PurpleAccount *)data;

	if ( ! account )
		return FALSE;

	if ( ! purple_account_get_connection(account) )
	{
		notify_plus_data.just_signed_on_accounts = g_list_remove(notify_plus_data.just_signed_on_accounts, account);
		return FALSE;
	}

	if ( ! purple_account_is_connected(account) )
		return TRUE;

	notify_plus_data.just_signed_on_accounts = g_list_remove(notify_plus_data.just_signed_on_accounts, account);
	return FALSE;
}

static void
event_connection_throttle(PurpleConnection *conn, gpointer data)
{
	PurpleAccount *account;

	if ( ! conn )
		return;

	account = purple_connection_get_account(conn);
	if ( ! account )
		return;

	notify_plus_data.just_signed_on_accounts = g_list_prepend(notify_plus_data.just_signed_on_accounts, account);
	g_timeout_add(5000, event_connection_throttle_cb, (gpointer)account);
}

static gboolean
plugin_load(PurplePlugin *plugin)
{
	void *conv_handle, *blist_handle, *conn_handle;

	if ( ( ! notify_is_initted() ) && ( ! notify_init("Pidgin") ) )
	{
		purple_debug_error(PLUGIN_ID, "libnotify not running!\n");
		return FALSE;
	}

	conv_handle = purple_conversations_get_handle();
	blist_handle = purple_blist_get_handle();
	conn_handle = purple_connections_get_handle();

	notify_plus_data.notifications = g_hash_table_new(NULL, NULL);

	purple_signal_connect(
		blist_handle, "buddy-signed-on", plugin,
		PURPLE_CALLBACK(notify_plus_buddy_signed_on_cb), NULL
		);

	purple_signal_connect(
		blist_handle, "buddy-signed-off", plugin,
		PURPLE_CALLBACK(notify_plus_buddy_signed_off_cb), NULL
		);

	purple_signal_connect(
		blist_handle, "buddy-status-changed", plugin,
		PURPLE_CALLBACK(notify_plus_buddy_status_changed_cb), NULL
		);

	purple_signal_connect(
		blist_handle, "buddy-idle-changed", plugin,
		PURPLE_CALLBACK(notify_plus_buddy_idle_changed_cb), NULL
		);

	purple_signal_connect(
		conv_handle, "received-im-msg", plugin,
		PURPLE_CALLBACK(notify_plus_new_im_msg_cb), NULL
		);

	purple_signal_connect(
		conv_handle, "received-chat-msg", plugin,
		PURPLE_CALLBACK(notify_plus_new_chat_msg_cb), NULL
		);


	purple_signal_connect(
		conn_handle, "signed-on", plugin,
		PURPLE_CALLBACK(event_connection_throttle), NULL
		);


	purple_signal_connect(
		blist_handle, "blist-node-extended-menu", plugin,
		PURPLE_CALLBACK(menu_add_notify_plus), NULL
		);


	#ifdef DEBUG
		send_notification("Pidgin-libtonify+", "Loaded", NULL, 5);
	#endif

	return TRUE;
}

static gboolean
plugin_unload(PurplePlugin *plugin)
{
	void *conv_handle, *blist_handle, *conn_handle;

	conv_handle = purple_conversations_get_handle();
	blist_handle = purple_blist_get_handle();
	conn_handle = purple_connections_get_handle();

	purple_signal_disconnect(
		blist_handle, "buddy-signed-on", plugin,
		PURPLE_CALLBACK(notify_plus_buddy_signed_on_cb)
		);

	purple_signal_disconnect(
		blist_handle, "buddy-signed-off", plugin,
		PURPLE_CALLBACK(notify_plus_buddy_signed_off_cb)
		);

	purple_signal_disconnect(
		blist_handle, "buddy-status-changed", plugin,
		PURPLE_CALLBACK(notify_plus_buddy_status_changed_cb)
		);

	purple_signal_disconnect(
		blist_handle, "buddy-idle-changed", plugin,
		PURPLE_CALLBACK(notify_plus_buddy_idle_changed_cb)
		);

	purple_signal_disconnect(
		conv_handle, "received-im-msg", plugin,
		PURPLE_CALLBACK(notify_plus_new_im_msg_cb)
		);

	purple_signal_disconnect(
		conv_handle, "received-chat-msg", plugin,
		PURPLE_CALLBACK(notify_plus_new_chat_msg_cb)
		);


	purple_signal_disconnect(
		conn_handle, "signed-on", plugin,
		PURPLE_CALLBACK(event_connection_throttle)
		);

	g_hash_table_unref(notify_plus_data.notifications);


	purple_signal_disconnect(
		blist_handle, "blist-node-extended-menu", plugin,
		PURPLE_CALLBACK(menu_add_notify_plus)
		);


	notify_uninit();

	return TRUE;
}

static PurplePluginUiInfo
prefs_info = {
	notify_plus_pref_frame,
	0,                                                               /* page num (Reserved) */
	NULL                                                             /* frame (Reserved) */
};

static PurplePluginInfo
info = {
	PURPLE_PLUGIN_MAGIC,                                              /* api version */
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_STANDARD,                                           /* type */
	PIDGIN_PLUGIN_TYPE,                                               /* ui requirement */
	0,                                                                /* flags */
	NULL,                                                             /* dependencies */
	PURPLE_PRIORITY_DEFAULT,                                          /* priority */

	PLUGIN_ID,                                                        /* id */
	NULL,                                                             /* name */
	PACKAGE_VERSION,                                                  /* version */
	NULL,                                                             /* summary */
	NULL,                                                             /* description */

	"Quentin \"Sardem FF7\" Glidic <sardemff7+pidgin@sardemff7.net>", /* author */
	"http://sardemff7.github.com/Pidgin-Libnotify-plus/",             /* homepage */

	plugin_load,                                                      /* load */
	plugin_unload,                                                    /* unload */
	NULL,	                                                          /* destroy */
	NULL,                                                             /* ui info */
	NULL,	                                                          /* extra info */
	&prefs_info	                                                  /* prefs info */
};

static void
init_plugin(PurplePlugin *plugin)
{
	notify_plus = plugin;

	#ifdef ENABLE_NLS
		bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
		bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	#endif

	info.name = "Libnotify+";
	info.summary = _("Displays popups via libnotify.");
	info.description = _("Displays popups via libnotify.");

	if ( purple_prefs_exists("/plugins/gtk/libnotify+") )
	{
		purple_prefs_add_none("/plugins/core/libnotify+");
		purple_prefs_add_none("/plugins/core/libnotify+");
		purple_prefs_add_bool("/plugins/core/libnotify+/new-msg", purple_prefs_get_bool("/plugins/gtk/libnotify+/new-msg"));
		purple_prefs_add_bool("/plugins/core/libnotify+/signed-on", purple_prefs_get_bool("/plugins/gtk/libnotify+/signed-on"));
		purple_prefs_add_bool("/plugins/core/libnotify+/signed-off", purple_prefs_get_bool("/plugins/gtk/libnotify+/signed-off"));
		purple_prefs_add_bool("/plugins/core/libnotify+/away", purple_prefs_get_bool("/plugins/gtk/libnotify+/away"));
		purple_prefs_add_bool("/plugins/core/libnotify+/idle", purple_prefs_get_bool("/plugins/gtk/libnotify+/idle"));
		purple_prefs_add_bool("/plugins/core/libnotify+/back", purple_prefs_get_bool("/plugins/gtk/libnotify+/back"));
		purple_prefs_add_bool("/plugins/core/libnotify+/status-message", purple_prefs_get_bool("/plugins/gtk/libnotify+/status-message"));
		purple_prefs_add_bool("/plugins/core/libnotify+/blocked", purple_prefs_get_bool("/plugins/gtk/libnotify+/blocked"));
		purple_prefs_add_bool("/plugins/core/libnotify+/new-conv-only", purple_prefs_get_bool("/plugins/gtk/libnotify+/new-conv-only"));
		purple_prefs_add_bool("/plugins/core/libnotify+/only-available", purple_prefs_get_bool("/plugins/gtk/libnotify+/only-available"));
		purple_prefs_add_int("/plugins/core/libnotify+/expire-timeout", purple_prefs_get_int("/plugins/gtk/libnotify+/expire-timeout"));
		purple_prefs_add_bool("/plugins/core/libnotify+/stack-notifications", purple_prefs_get_bool("/plugins/gtk/libnotify+/stack-notifications"));
		purple_prefs_remove("/plugins/gtk/libnotify+");
	}
	else if ( ! purple_prefs_exists("/plugins/core/libnotify+") )
	{
		purple_prefs_add_none("/plugins/core/libnotify+");
		purple_prefs_add_bool("/plugins/core/libnotify+/new-msg", TRUE);
		purple_prefs_add_bool("/plugins/core/libnotify+/signed-on", TRUE);
		purple_prefs_add_bool("/plugins/core/libnotify+/signed-off", FALSE);
		purple_prefs_add_bool("/plugins/core/libnotify+/away", TRUE);
		purple_prefs_add_bool("/plugins/core/libnotify+/idle", TRUE);
		purple_prefs_add_bool("/plugins/core/libnotify+/back", TRUE);
		purple_prefs_add_bool("/plugins/core/libnotify+/status-message", FALSE);
		purple_prefs_add_bool("/plugins/core/libnotify+/blocked", TRUE);
		purple_prefs_add_bool("/plugins/core/libnotify+/new-conv-only", FALSE);
		purple_prefs_add_bool("/plugins/core/libnotify+/only-available", FALSE);
		purple_prefs_add_int("/plugins/core/libnotify+/expire-timeout", -1);
		purple_prefs_add_bool("/plugins/core/libnotify+/stack-notifications", FALSE);
	}
}

PURPLE_INIT_PLUGIN(notify_plus, init_plugin, info)
