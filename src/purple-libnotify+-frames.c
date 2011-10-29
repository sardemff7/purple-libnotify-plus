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

#include "purple-libnotify+-frames.h"

PurplePluginPrefFrame *
notify_plus_pref_frame(PurplePlugin *plugin)
{
	PurplePluginPrefFrame *frame;
	PurplePluginPref *pref;

	frame = purple_plugin_pref_frame_new();


	pref = purple_plugin_pref_new_with_label(
		_("Notify me on:")
		);
	purple_plugin_pref_frame_add(frame, pref);

	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/core/libnotify+/new-msg",
		_("New messages")
		);
	purple_plugin_pref_frame_add(frame, pref);

	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/core/libnotify+/signed-on",
		_("Buddy signing on")
		);
	purple_plugin_pref_frame_add(frame, pref);

	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/core/libnotify+/signed-off",
		_("Buddy signing off")
		);
	purple_plugin_pref_frame_add(frame, pref);

	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/core/libnotify+/away",
		_("Buddy going away")
		);
	purple_plugin_pref_frame_add(frame, pref);

	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/core/libnotify+/idle",
		_("Buddy going idle")
		);
	purple_plugin_pref_frame_add(frame, pref);

	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/core/libnotify+/back",
		_("Buddy coming back")
		);
	purple_plugin_pref_frame_add(frame, pref);

	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/core/libnotify+/status-message",
		_("Status message change (or removal)")
		);
	purple_plugin_pref_frame_add(frame, pref);


	pref = purple_plugin_pref_new_with_label(
		_("Restrictions:")
		);
	purple_plugin_pref_frame_add(frame, pref);

	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/core/libnotify+/blocked",
		_("Even for a blocked buddy")
		);
	purple_plugin_pref_frame_add(frame, pref);

	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/core/libnotify+/new-conv-only",
		_("Only from new conversation")
		);
	purple_plugin_pref_frame_add(frame, pref);

	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/core/libnotify+/only-available",
		_("Only when available")
		);
	purple_plugin_pref_frame_add(frame, pref);


	pref = purple_plugin_pref_new_with_label(
		_("Notification settings:")
		);
	purple_plugin_pref_frame_add(frame, pref);

	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/core/libnotify+/expire-timeout",
		_("Expire timeout (1-120, 0 for never, -1 for auto)")
		);
	purple_plugin_pref_frame_add(frame, pref);
	purple_plugin_pref_set_bounds(pref, -1, 120);

	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/core/libnotify+/stack-notifications",
		_("Stack notifications (do not ignore a buddy action if a notification is already attached to)")
		);
	purple_plugin_pref_frame_add(frame, pref);


	return frame;
}

#define NOTIFY_PLUS_GET_CONTACT(node) \
	if ( PURPLE_BLIST_NODE_IS_BUDDY(node) )\
		node = PURPLE_BLIST_NODE(purple_buddy_get_contact(PURPLE_BUDDY(node)));

static void
deactivate_reset(PurpleBlistNode *node, gpointer data)
{
	NOTIFY_PLUS_GET_CONTACT(node);

	purple_blist_node_remove_setting(node, PACKAGE_NAME"/deactivate");
}

static void
deactivate_set(PurpleBlistNode *node, gpointer data)
{
	NOTIFY_PLUS_GET_CONTACT(node);

	purple_blist_node_set_int(node, PACKAGE_NAME"/deactivate", 1);
}

static void
deactivate_unset(PurpleBlistNode *node, gpointer data)
{
	NOTIFY_PLUS_GET_CONTACT(node);

	purple_blist_node_set_int(node, PACKAGE_NAME"/deactivate", -1);
}

void
menu_add_notify_plus(PurpleBlistNode *node, GList **menu)
{
	NOTIFY_PLUS_GET_CONTACT(node);

	if ( ( purple_blist_node_get_flags(node) & PURPLE_BLIST_NODE_FLAG_NO_SAVE ) || ( PURPLE_BLIST_NODE_IS_CHAT(node) ) )
		return;

	gint current = purple_blist_node_get_int(node, PACKAGE_NAME"/deactivate");

	GList *submenu = NULL;

	if ( current != 0 )
		submenu = g_list_prepend(submenu, purple_menu_action_new(_("Use defaults"), PURPLE_CALLBACK(deactivate_reset), NULL, NULL));
	else if ( PURPLE_BLIST_NODE_IS_CONTACT(node) )
	{
		PurpleBlistNode *group = PURPLE_BLIST_NODE(purple_buddy_get_group(purple_contact_get_priority_buddy(PURPLE_CONTACT(node))));
		current = purple_blist_node_get_int(group, PACKAGE_NAME"/deactivate");
	}

	PurpleMenuAction *action = NULL;
	if ( current == 1 )
		action = purple_menu_action_new(_("Activate popup"), PURPLE_CALLBACK(deactivate_unset), NULL, NULL);
	else
		action = purple_menu_action_new(_("Deactivate popup"), PURPLE_CALLBACK(deactivate_set), NULL, NULL);

	submenu = g_list_prepend(submenu, action);

	(*menu) = g_list_append(*menu, purple_menu_action_new("Libnotify+", NULL, NULL, g_list_reverse(submenu)));
}
