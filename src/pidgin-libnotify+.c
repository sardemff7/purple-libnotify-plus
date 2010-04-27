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

#include "pidgin-libnotify+.h"

static PurplePlugin *notify_plus = NULL;
static GList *just_signed_on_accounts = NULL;


#include "pidgin-libnotify+-utils.c"
#include "pidgin-libnotify+-frames.c"


static void
notify_plus_buddy_signed_on_cb(
	PurpleBuddy *buddy,
	gpointer data)
{
	if ( ( ! purple_prefs_get_bool("/plugins/gtk/libnotify+/signed-on") ) || ( ! is_buddy_notify(buddy) ) )
		return;
	
	gchar *name = get_best_buddy_name(buddy);
	send_notification(name, _("sign on"), buddy);
	g_free(name);
}

static void
notify_plus_buddy_signed_off_cb(
	PurpleBuddy *buddy,
	gpointer data)
{
	if ( ( ! purple_prefs_get_bool("/plugins/gtk/libnotify+/signed-off") ) || ( ! is_buddy_notify(buddy) ) )
		return;
	
	gchar *name = get_best_buddy_name(buddy);
	send_notification(name, _("sign off"), buddy);
	g_free(name);
}

static void
notify_plus_buddy_status_changed_cb(
	PurpleBuddy *buddy,
	PurpleStatus *oldstatus,
	PurpleStatus *newstatus,
	gpointer data)
{
	const char *action;
	if ( ( purple_status_is_available(oldstatus) ) && ( ! purple_status_is_available(newstatus) ) )
	{
		if ( ! purple_prefs_get_bool("/plugins/gtk/libnotify+/away") )
			return;
		action = _("go away");
	}
	else if ( ( ! purple_status_is_available(oldstatus) ) && ( purple_status_is_available(newstatus) ) )
	{
		action = _("come back");
	}
	else
		return;
	
	if ( ! is_buddy_notify(buddy) )
		return;
	
	gchar *name = get_best_buddy_name(buddy);
	send_notification(name, action, buddy);
	g_free(name);
}

static void
notify_plus_buddy_idle_changed_cb(
	PurpleBuddy *buddy,
	gboolean oldidle,
	gboolean newidle,
	gpointer data)
{
	if ( ( ! purple_prefs_get_bool("/plugins/gtk/libnotify+/idle") ) || ( ! is_buddy_notify(buddy) ) )
		return;
	
	gchar *name = get_best_buddy_name(buddy);
	send_notification(name, ( newidle ) ? ( _("go idle") ) : ( _("come back idle") ), buddy);
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
	if ( ( ! purple_prefs_get_bool("/plugins/gtk/libnotify+/new-msg") ) || ( ! buddy ) || ( ! is_buddy_notify(buddy) ) )
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
	if ( ( ! purple_prefs_get_bool("/plugins/gtk/libnotify+/new-msg") ) || ( ! buddy ) || ( ! is_buddy_notify(buddy) ) )
		return;
	
	gchar *name = get_best_buddy_name(buddy);
	gchar *title = g_strdup_printf(_("%s says"), name);
	g_free(name);
	
	gchar *body = purple_markup_strip_html(message);
	
	send_notification(title, body, buddy);
	
	g_free(title);
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
		just_signed_on_accounts = g_list_remove(just_signed_on_accounts, account);
		return FALSE;
	}
	
	if ( ! purple_account_is_connected(account) )
		return TRUE;
	
	just_signed_on_accounts = g_list_remove(just_signed_on_accounts, account);
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
	
	just_signed_on_accounts = g_list_prepend(just_signed_on_accounts, account);
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
	
	#ifdef MODIFY_NOTIFY
	buddy_hash = g_hash_table_new(NULL, NULL);
	#endif /* MODIFY_NOTIFY */
	
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
		send_notification("Pidgin-libtonify+", "Loaded", NULL);
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
	
	#ifdef MODIFY_NOTIFY
	g_hash_table_destroy(buddy_hash);
	#endif /* MODIFY_NOTIFY */
	
	
	purple_signal_disconnect(
		blist_handle, "blist-node-extended-menu", plugin,
		PURPLE_CALLBACK(menu_add_notify_plus)
		);
	
	
	notify_uninit();
	
	return TRUE;
}

static PurplePluginUiInfo prefs_info = {
	notify_plus_pref_frame,
	0,						/* page num (Reserved) */
	NULL					/* frame (Reserved) */
};

static PurplePluginInfo info = {
	PURPLE_PLUGIN_MAGIC,									/* api version */
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_STANDARD,									/* type */
	0,														/* ui requirement */
	0,														/* flags */
	NULL,													/* dependencies */
	PURPLE_PRIORITY_DEFAULT,								/* priority */

	PLUGIN_ID,												/* id */
	NULL,													/* name */
	VERSION,												/* version */
	NULL,													/* summary */
	NULL,													/* description */

	"Sardem FF7 <sardemff7.pub@gmail.com>",					/* author */
	"",		/* homepage */

	plugin_load,	/* load */
	plugin_unload,	/* unload */
	NULL,			/* destroy */
	NULL,			/* ui info */
	NULL,			/* extra info */
	&prefs_info		/* prefs info */
};

static void
init_plugin(PurplePlugin *plugin)
{
	notify_plus = plugin;
	
	#ifdef ENABLE_NLS
		bindtextdomain(PACKAGE, LOCALEDIR);
		bind_textdomain_codeset(PACKAGE, "UTF-8");
	#endif
	
	info.name = _("Libnotify+");
	info.summary = _("Displays popups via libnotify.");
	info.description = _("Displays popups via libnotify.");
	
	purple_prefs_add_none("/plugins/gtk/libnotify+");
	purple_prefs_add_bool("/plugins/gtk/libnotify+/new-msg", TRUE);
	purple_prefs_add_bool("/plugins/gtk/libnotify+/signed-on", TRUE);
	purple_prefs_add_bool("/plugins/gtk/libnotify+/signed-off", FALSE);
	purple_prefs_add_bool("/plugins/gtk/libnotify+/away", TRUE);
	purple_prefs_add_bool("/plugins/gtk/libnotify+/idle", TRUE);
	purple_prefs_add_bool("/plugins/gtk/libnotify+/back", TRUE);
	purple_prefs_add_bool("/plugins/gtk/libnotify+/blocked", TRUE);
	purple_prefs_add_bool("/plugins/gtk/libnotify+/new-conv-only", FALSE);
	purple_prefs_add_bool("/plugins/gtk/libnotify+/only-available", FALSE);
}

PURPLE_INIT_PLUGIN(notify_plus, init_plugin, info)
