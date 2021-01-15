/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2007-2010 Jannis Pohlmann <jannis@expidus.org>
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

#ifndef __MARKON_MENU_SEPARATOR_H__
#define __MARKON_MENU_SEPARATOR_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define MARKON_TYPE_MENU_SEPARATOR            (markon_menu_separator_get_type())
#define MARKON_MENU_SEPARATOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MARKON_TYPE_MENU_SEPARATOR, MarkonMenuSeparator))
#define MARKON_MENU_SEPARATOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MARKON_TYPE_MENU_SEPARATOR, MarkonMenuSeparatorClass))
#define MARKON_IS_MENU_SEPARATOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MARKON_TYPE_MENU_SEPARATOR))
#define MARKON_IS_MENU_SEPARATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MARKON_TYPE_MENU_SEPARATOR))
#define MARKON_MENU_SEPARATOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MARKON_TYPE_MENU_SEPARATOR, MarkonMenuSeparatorClass))

typedef struct _MarkonMenuSeparatorClass MarkonMenuSeparatorClass;
typedef struct _MarkonMenuSeparator      MarkonMenuSeparator;

struct _MarkonMenuSeparatorClass
{
  GObjectClass __parent__;
};

struct _MarkonMenuSeparator
{
  GObject __parent__;
};



GType                markon_menu_separator_get_type    (void) G_GNUC_CONST;

MarkonMenuSeparator *markon_menu_separator_get_default (void);

G_END_DECLS

#endif /* !__MARKON_MENU_SEPARATOR_H__ */
