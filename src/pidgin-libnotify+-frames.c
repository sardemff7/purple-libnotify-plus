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

static PurplePluginPrefFrame *
notify_plus_pref_frame(PurplePlugin *plugin)
{
	PurplePluginPrefFrame *frame;
	PurplePluginPref *pref;
	
	frame = purple_plugin_pref_frame_new();
	
	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/gtk/libnotify+/new-msg",
		_("Notify on new messages")
		);
	purple_plugin_pref_frame_add(frame, pref);
	
	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/gtk/libnotify+/signed-on",
		_("Notify when a buddy sign on")
		);
	purple_plugin_pref_frame_add(frame, pref);
	
	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/gtk/libnotify+/signed-off",
		_("Notify when a buddy sign off")
		);
	purple_plugin_pref_frame_add(frame, pref);
	
	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/gtk/libnotify+/away",
		_("Notify when a buddy go away")
		);
	purple_plugin_pref_frame_add(frame, pref);
	
	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/gtk/libnotify+/idle",
		_("Notify when a buddy go idle")
		);
	purple_plugin_pref_frame_add(frame, pref);
	
	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/gtk/libnotify+/back",
		_("Notify when a buddy come back")
		);
	purple_plugin_pref_frame_add(frame, pref);
	
	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/gtk/libnotify+/blocked",
		_("Notify if a buddy is blocked")
		);
	purple_plugin_pref_frame_add(frame, pref);
	
	
	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/gtk/libnotify+/new-conv-only",
		_("Notify only from new conversation")
		);
	purple_plugin_pref_frame_add(frame, pref);
	
	
	pref = purple_plugin_pref_new_with_name_and_label(
		"/plugins/gtk/libnotify+/only-available",
		_("Notify only when available")
		);
	purple_plugin_pref_frame_add(frame, pref);
	
	
	return frame;
}

#define NOTIFY_PLUS_GET_CONTACT(node) \
	if ( PURPLE_BLIST_NODE_IS_BUDDY(node) )\
		node = PURPLE_BLIST_NODE(purple_buddy_get_contact(PURPLE_BUDDY(node)));

static void
reset_no_notify(PurpleBlistNode *node, gpointer data)
{
	NOTIFY_PLUS_GET_CONTACT(node);
	
	purple_blist_node_remove_setting(node, "no-notify");
	
	if ( PURPLE_BLIST_NODE_IS_GROUP(node) )
	{
		GList *already_done = NULL;
		PurpleBlistNode *contact = purple_blist_node_get_first_child(node);
		do
		{
			NOTIFY_PLUS_GET_CONTACT(contact);
			
			if ( ! g_list_find(already_done, contact) )
			{
				int no = purple_blist_node_get_int(contact, "no-notify");
				int save = purple_blist_node_get_int(contact, "save_no-notify");
				
				if ( no == -1 )
					purple_blist_node_set_int(contact, "save_no-notify", -1);
				else
					purple_blist_node_remove_setting(contact, "save_no-notify");
				
				if ( save == 1 )
					purple_blist_node_set_int(contact, "no-notify", 1);
				else
					purple_blist_node_remove_setting(contact, "no-notify");
				
				already_done = g_list_append(already_done, contact);
			}
		} while ( ( contact = purple_blist_node_get_sibling_next(contact) ) != NULL );
		g_list_free(already_done);
	}
}

static void
set_no_notify(PurpleBlistNode *node, gpointer data)
{
	NOTIFY_PLUS_GET_CONTACT(node);
	
	purple_blist_node_set_int(node, "no-notify", 1);
	
	if ( PURPLE_BLIST_NODE_IS_GROUP(node) )
	{
		GList *already_done = NULL;
		PurpleBlistNode *contact = purple_blist_node_get_first_child(node);
		do
		{
			NOTIFY_PLUS_GET_CONTACT(contact);
			
			if ( ! g_list_find(already_done, contact) )
			{
				int no = purple_blist_node_get_int(contact, "no-notify");
				int save = purple_blist_node_get_int(contact, "save_no-notify");
				
				if ( no == 1 )
					purple_blist_node_set_int(contact, "save_no-notify", 1);
				else
					purple_blist_node_remove_setting(contact, "save_no-notify");
				
				if ( save == -1 )
					purple_blist_node_set_int(contact, "no-notify", -1);
				else
					purple_blist_node_remove_setting(contact, "no-notify");
				
				already_done = g_list_append(already_done, contact);
			}
		} while ( ( contact = purple_blist_node_get_sibling_next(contact) ) != NULL );
		g_list_free(already_done);
	}
}

static void
force_notify(PurpleBlistNode *node, gpointer data)
{
	NOTIFY_PLUS_GET_CONTACT(node);
	
	purple_blist_node_set_int(node, "no-notify", -1);
}

static void
reset_contact(PurpleBlistNode *node, gpointer data)
{
	NOTIFY_PLUS_GET_CONTACT(node);
	
	purple_blist_node_remove_setting(node, "no-notify");
	purple_blist_node_remove_setting(node, "save_no-notify");
}

static void
reset_all_contacts_in_group(PurpleBlistNode *node, gpointer data)
{
	GList *already_done = NULL;
	PurpleBlistNode *contact = purple_blist_node_get_first_child(node);
	do
	{
		NOTIFY_PLUS_GET_CONTACT(contact);
		
		if ( ! g_list_find(already_done, contact) )
		{
			reset_contact(contact, NULL);
			already_done = g_list_append(already_done, contact);
		}
	} while ( ( contact = purple_blist_node_get_sibling_next(contact) ) != NULL );
	g_list_free(already_done);
}

static void
menu_add_notify_plus(PurpleBlistNode *node, GList **menu)
{
	NOTIFY_PLUS_GET_CONTACT(node);
	
	if ( ( purple_blist_node_get_flags(node) & PURPLE_BLIST_NODE_FLAG_NO_SAVE ) || ( PURPLE_BLIST_NODE_IS_CHAT(node) ) )
		return;
	
	(*menu) = g_list_append(*menu, NULL);
	
	PurpleBlistNode *group = NULL;
	if ( PURPLE_BLIST_NODE_IS_CONTACT(node) )
		group = PURPLE_BLIST_NODE(purple_buddy_get_group(purple_contact_get_priority_buddy(PURPLE_CONTACT(node))));
	
	PurpleMenuAction *action;
	if ( purple_blist_node_get_int(node, "no-notify") == 1 )
		action = purple_menu_action_new(_("Activate libnotify+ popup"), PURPLE_CALLBACK(reset_no_notify), NULL, NULL);
	else if ( ( group ) && ( purple_blist_node_get_int(group, "no-notify") == 1 ) && ( purple_blist_node_get_int(node, "no-notify") == 0 ) )
		action = purple_menu_action_new(_("Activate libnotify+ popup"), PURPLE_CALLBACK(force_notify), NULL, NULL);
	else
		action = purple_menu_action_new(_("Disactivate libnotify+ popup"), PURPLE_CALLBACK(set_no_notify), NULL, NULL);
	
	if ( action )
		(*menu) = g_list_append(*menu, action);
	
	PurpleMenuAction *action_reset = NULL;
	if ( PURPLE_BLIST_NODE_IS_GROUP(node) )
		action_reset = purple_menu_action_new(_("Reset all"), PURPLE_CALLBACK(reset_all_contacts_in_group), NULL, NULL);
	else if ( ( PURPLE_BLIST_NODE_IS_CONTACT(node) ) &&
		( ( purple_blist_node_get_int(node, "no-notify") != 0 ) || ( purple_blist_node_get_int(node, "save_no-notify") != 0 ) ) )
		action_reset = purple_menu_action_new(_("Reset"), PURPLE_CALLBACK(reset_contact), NULL, NULL);
	if ( action_reset )
		(*menu) = g_list_append(*menu, action_reset);
}
