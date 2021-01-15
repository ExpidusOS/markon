/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2009 Jannis Pohlmann <jannis@expidus.org>
 * Copyright (c) 2009 Nick Schermer <nick@expidus.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#if !defined(MARKON_INSIDE_MARKON_H) && !defined(MARKON_COMPILATION)
#error "Only <markon/markon.h> can be included directly. This file may disappear or change contents."
#endif

#ifndef __MARKON_PRIVATE_H__
#define __MARKON_PRIVATE_H__

G_BEGIN_DECLS

/* Macro for new g_?list_free_full function */
#define _markon_g_slist_free_full(list,free_func) \
  g_slist_free_full (list, (GDestroyNotify) free_func)
#define _markon_g_list_free_full(list,free_func) \
  g_list_free_full (list, (GDestroyNotify) free_func)

/* if XDG_MENU_PREFIX is not set, default to "expidus-" so markon doesn't
 * break when expidus is not started with startexpidus1 */
#define MARKON_DEFAULT_MENU_PREFIX "expidus-"

GFile    *_markon_file_new_for_unknown_input    (const gchar *path,
                                                 GFile       *parent);

GFile    *_markon_file_new_relative_to_file     (const gchar *path,
                                                 GFile       *file);

gchar    *_markon_file_get_uri_relative_to_file (const gchar *path,
                                                 GFile       *file);

G_END_DECLS

#endif /* !__MARKON_PRIVATE_H__ */
