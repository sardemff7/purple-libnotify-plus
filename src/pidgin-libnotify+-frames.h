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

#ifndef __PIDGIN_LIBNOTIFY_PLUS_FRAMES_H__
#define __PIDGIN_LIBNOTIFY_PLUS_FRAMES_H__

PurplePluginPrefFrame *notify_plus_pref_frame(PurplePlugin *plugin);
void menu_add_notify_plus(PurpleBlistNode *node, GList **menu);

#endif /* __PIDGIN_LIBNOTIFY_PLUS_FRAMES_H__ */
