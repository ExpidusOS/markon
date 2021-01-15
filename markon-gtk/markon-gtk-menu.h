/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2013 Nick Schermer <nick@expidus.org>
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

#if !defined(MARKON_INSIDE_MARKON_GTK_H) && !defined(MARKON_COMPILATION)
#error "Only <markon-gtk/markon-gtk.h> can be included directly. This file may disappear or change contents."
#endif

#ifndef __MARKON_GTK_MENU_H__
#define __MARKON_GTK_MENU_H__

#include <gtk/gtk.h>
#include <markon/markon.h>

G_BEGIN_DECLS

#define MARKON_GTK_TYPE_MENU            (markon_gtk_menu_get_type ())
#define MARKON_GTK_MENU(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MARKON_GTK_TYPE_MENU, MarkonGtkMenu))
#define MARKON_GTK_MENU_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MARKON_GTK_TYPE_MENU, MarkontkMenuClass))
#define MARKON_GTK_IS_MENU(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MARKON_GTK_TYPE_MENU))
#define MARKON_GTK_IS_MENU_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MARKON_GTK_TYPE_MENU))
#define MARKON_GTK_MENU_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MARKON_GTK_TYPE_MENU, MarkontkMenuClass))

typedef struct _MarkonGtkMenuPrivate MarkonGtkMenuPrivate;
typedef struct _MarkonGtkMenuClass   MarkonGtkMenuClass;
typedef struct _MarkonGtkMenu        MarkonGtkMenu;

struct _MarkonGtkMenuClass
{
  GtkMenuClass __parent__;
};

struct _MarkonGtkMenu
{
  GtkMenu              __parent__;

  /* < private > */
  MarkonGtkMenuPrivate *priv;
};

GType                markon_gtk_menu_get_type                 (void) G_GNUC_CONST;

GtkWidget           *markon_gtk_menu_new                      (MarkonMenu    *markon_menu) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

void                 markon_gtk_menu_set_menu                 (MarkonGtkMenu *menu,
                                                               MarkonMenu    *markon_menu);

MarkonMenu          *markon_gtk_menu_get_menu                 (MarkonGtkMenu *menu);

void                 markon_gtk_menu_set_show_generic_names   (MarkonGtkMenu *menu,
                                                               gboolean       show_generic_names);
gboolean             markon_gtk_menu_get_show_generic_names   (MarkonGtkMenu *menu);

void                 markon_gtk_menu_set_show_menu_icons      (MarkonGtkMenu *menu,
                                                               gboolean       show_menu_icons);
gboolean             markon_gtk_menu_get_show_menu_icons      (MarkonGtkMenu *menu);

void                 markon_gtk_menu_set_show_tooltips        (MarkonGtkMenu *menu,
                                                               gboolean       show_tooltips);
gboolean             markon_gtk_menu_get_show_tooltips        (MarkonGtkMenu *menu);

void                 markon_gtk_menu_set_show_desktop_actions (MarkonGtkMenu *menu,
                                                               gboolean       show_desktop_actions);
gboolean             markon_gtk_menu_get_show_desktop_actions (MarkonGtkMenu *menu);

GtkMenu             *markon_gtk_menu_get_desktop_actions_menu (MarkonMenuItem *item);

void                 markon_gtk_menu_set_right_click_edits    (MarkonGtkMenu *menu,
                                                               gboolean       enable_right_click_edits);
gboolean             markon_gtk_menu_get_right_click_edits    (MarkonGtkMenu *menu);

G_END_DECLS

#endif /* !__MARKON_GTK_MENU_H__ */
