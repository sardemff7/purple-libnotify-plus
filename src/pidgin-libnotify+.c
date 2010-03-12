/*
 * Pidgin-libnotify+ - Provide libnotify interface to Pidgin
 * Copyright (C) 2010 Sardem FF7
 * 
 * This file is part of Pidgin-libnotify+.
 * 
 * Pidgin-libnotify+ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Pidgin-libnotify+ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Pidgin-libnotify+.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef PURPLE_PLUGINS
#define PURPLE_PLUGINS
#endif

#include <pidgin.h>
#include <version.h>
#include <debug.h>
#include <util.h>
#include <privacy.h>

/* for pidgin_create_prpl_icon */
#include <gtkutils.h>

#include <libnotify/notify.h>

#include <string.h>

#define PLUGIN_ID "pidgin-libnotify+"

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
	"http://sourceforge.net/projects/gaim-libnotify/",		/* homepage */

	plugin_load,			/* load */
	plugin_unload,			/* unload */
	NULL,					/* destroy */
	NULL,					/* ui info */
	NULL,					/* extra info */
	&prefs_info				/* prefs info */
};

static void init_plugin(PurplePlugin *plugin)
{
	bindtextdomain(PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(PACKAGE, "UTF-8");

	info.name = _("Libnotify+");
	info.summary = _("Displays popups via libnotify.");
	info.description = _("Pidgin-libnotify+:\nDisplays popups via libnotify.");

	purple_prefs_add_none("/plugins/gtk/libnotify+");
	purple_prefs_add_bool("/plugins/gtk/libnotify+/newmsg", TRUE);
	purple_prefs_add_bool("/plugins/gtk/libnotify+/blocked", TRUE);
	purple_prefs_add_bool("/plugins/gtk/libnotify+/newconvonly", FALSE);
	purple_prefs_add_bool("/plugins/gtk/libnotify+/signon", TRUE);
	purple_prefs_add_bool("/plugins/gtk/libnotify+/signoff", FALSE);
	purple_prefs_add_bool("/plugins/gtk/libnotify+/only_available", FALSE);
}

PURPLE_INIT_PLUGIN(notify, init_plugin, info)

