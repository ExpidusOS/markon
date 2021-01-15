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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <markon/markon-menu-element.h>
#include <markon/markon-menu-separator.h>



static void         markon_menu_separator_element_init                    (MarkonMenuElementIface   *iface);
static void         markon_menu_separator_finalize                        (GObject                  *object);

static const gchar *markon_menu_separator_get_element_name                (MarkonMenuElement        *element);
static const gchar *markon_menu_separator_get_element_comment             (MarkonMenuElement        *element);
static const gchar *markon_menu_separator_get_element_icon_name           (MarkonMenuElement        *element);
static gboolean     markon_menu_separator_get_element_visible             (MarkonMenuElement        *element);
static gboolean     markon_menu_separator_get_element_show_in_environment (MarkonMenuElement        *element);
static gboolean     markon_menu_separator_get_element_no_display          (MarkonMenuElement        *element);
static gboolean     markon_menu_separator_get_element_equal               (MarkonMenuElement        *element,
                                                                           MarkonMenuElement        *other);



G_DEFINE_TYPE_WITH_CODE (MarkonMenuSeparator, markon_menu_separator, G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE (MARKON_TYPE_MENU_ELEMENT, markon_menu_separator_element_init))



static void
markon_menu_separator_class_init (MarkonMenuSeparatorClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = markon_menu_separator_finalize;
}



static void
markon_menu_separator_element_init (MarkonMenuElementIface *iface)
{
  iface->get_name = markon_menu_separator_get_element_name;
  iface->get_comment = markon_menu_separator_get_element_comment;
  iface->get_icon_name = markon_menu_separator_get_element_icon_name;
  iface->get_visible = markon_menu_separator_get_element_visible;
  iface->get_show_in_environment = markon_menu_separator_get_element_show_in_environment;
  iface->get_no_display = markon_menu_separator_get_element_no_display;
  iface->equal = markon_menu_separator_get_element_equal;
}



static void
markon_menu_separator_init (MarkonMenuSeparator *separator)
{
}



static void
markon_menu_separator_finalize (GObject *object)
{
  (*G_OBJECT_CLASS (markon_menu_separator_parent_class)->finalize) (object);
}



/**
 * markon_menu_separator_get_default: (constructor)
 *
 * Returns a new #MarkonMenuSeparator. The returned object
 * should be unreffed with g_object_unref() when no longer needed.
 *
 * Returns: (transfer full): a new #MarkonMenuSeparator.
 */
MarkonMenuSeparator*
markon_menu_separator_get_default (void)
{
  MarkonMenuSeparator *separator = NULL;

  if (G_UNLIKELY (separator == NULL))
    {
      /* Create a new separator */
      separator = g_object_new (MARKON_TYPE_MENU_SEPARATOR, NULL);
      g_object_add_weak_pointer (G_OBJECT (separator), (gpointer) &separator);
    }
  else
    {
      /* Take a reference */
      g_object_ref (G_OBJECT (separator));
    }

  return separator;
}



static const gchar*
markon_menu_separator_get_element_name (MarkonMenuElement *element)
{
  return NULL;
}



static const gchar*
markon_menu_separator_get_element_comment (MarkonMenuElement *element)
{
  return NULL;
}



static const gchar*
markon_menu_separator_get_element_icon_name (MarkonMenuElement *element)
{
  return NULL;
}



static gboolean
markon_menu_separator_get_element_visible (MarkonMenuElement *element)
{
  return TRUE;
}



static gboolean
markon_menu_separator_get_element_show_in_environment (MarkonMenuElement *element)
{
  return TRUE;
}



static gboolean
markon_menu_separator_get_element_no_display (MarkonMenuElement *element)
{
  return FALSE;
}



static gboolean
markon_menu_separator_get_element_equal (MarkonMenuElement *element,
                                         MarkonMenuElement *other)
{
  /* FIXME this is inherently broken as the separator is a singleton class */
  return FALSE;
}
