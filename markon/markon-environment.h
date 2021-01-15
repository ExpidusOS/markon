/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2007-2009 Jannis Pohlmann <jannis@expidus.org>
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

#ifndef __MARKON_MENU_ENVIRONMENT_H__
#define __MARKON_MENU_ENVIRONMENT_H__

/**
 * MARKON_ENVIRONMENT_EXPIDUS:
 *
 * Macro for markon_set_environment or markon_set_environment_xdg
 * to set the Expidus Desktop Environment.
 *
 * Since: 0.3.0
 **/
#define MARKON_ENVIRONMENT_EXPIDUS "EXPIDUS"

#include <glib.h>

G_BEGIN_DECLS

void         markon_set_environment     (const gchar *env);

const gchar *markon_get_environment     (void);

void         markon_set_environment_xdg (const gchar *fallback_env);

G_END_DECLS

#endif /* !__MARKON_MENU_ENVIRONMENT_H__ */
