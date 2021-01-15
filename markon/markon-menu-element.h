/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2006-2009 Jannis Pohlmann <jannis@expidus.org>
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

#if !defined (MARKON_INSIDE_MARKON_H) && !defined (MARKON_COMPILATION)
#error "Only <markon/markon.h> can be included directly. This file may disappear or change contents."
#endif

#ifndef __MARKON_MENU_ELEMENT_H__
#define __MARKON_MENU_ELEMENT_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define MARKON_TYPE_MENU_ELEMENT           (markon_menu_element_get_type ())
#define MARKON_MENU_ELEMENT(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), MARKON_TYPE_MENU_ELEMENT, MarkonMenuElement))
#define MARKON_IS_MENU_ELEMENT(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MARKON_TYPE_MENU_ELEMENT))
#define MARKON_MENU_ELEMENT_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), MARKON_TYPE_MENU_ELEMENT, MarkonMenuElementIface))

typedef struct _MarkonMenuElement      MarkonMenuElement;
typedef struct _MarkonMenuElementIface MarkonMenuElementIface;

struct _MarkonMenuElementIface
{
  GTypeInterface __parent__;

  /* Virtual methods */
  const gchar *(*get_name)                (MarkonMenuElement *element);
  const gchar *(*get_comment)             (MarkonMenuElement *element);
  const gchar *(*get_icon_name)           (MarkonMenuElement *element);
  gboolean     (*get_visible)             (MarkonMenuElement *element);
  gboolean     (*get_show_in_environment) (MarkonMenuElement *element);
  gboolean     (*get_no_display)          (MarkonMenuElement *element);
  gboolean     (*equal)                   (MarkonMenuElement *element,
                                           MarkonMenuElement *other);
};

GType        markon_menu_element_get_type                (void) G_GNUC_CONST;

const gchar *markon_menu_element_get_name                (MarkonMenuElement *element);
const gchar *markon_menu_element_get_comment             (MarkonMenuElement *element);
const gchar *markon_menu_element_get_icon_name           (MarkonMenuElement *element);
gboolean     markon_menu_element_get_visible             (MarkonMenuElement *element);
gboolean     markon_menu_element_get_show_in_environment (MarkonMenuElement *element);
gboolean     markon_menu_element_get_no_display          (MarkonMenuElement *element);
gboolean     markon_menu_element_equal                   (MarkonMenuElement *a,
                                                          MarkonMenuElement *b);

G_END_DECLS

#endif /* !__MARKON_MENU_ELEMENT_H__ */
