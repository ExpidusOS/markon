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

#if !defined(MARKON_INSIDE_MARKON_H) && !defined(MARKON_COMPILATION)
#error "Only <markon/markon.h> can be included directly. This file may disappear or change contents."
#endif

#ifndef __MARKON_MENU_H__
#define __MARKON_MENU_H__

#include <gio/gio.h>
#include <markon/markon-menu-item-pool.h>

G_BEGIN_DECLS

#define MARKON_TYPE_MENU            (markon_menu_get_type ())
#define MARKON_MENU(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MARKON_TYPE_MENU, MarkonMenu))
#define MARKON_MENU_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MARKON_TYPE_MENU, MarkonMenuClass))
#define MARKON_IS_MENU(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MARKON_TYPE_MENU))
#define MARKON_IS_MENU_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MARKON_TYPE_MENU))
#define MARKON_MENU_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MARKON_TYPE_MENU, MarkonMenuClass))

typedef struct _MarkonMenuPrivate MarkonMenuPrivate;
typedef struct _MarkonMenuClass   MarkonMenuClass;
typedef struct _MarkonMenu        MarkonMenu;

GType                markon_menu_get_type           (void) G_GNUC_CONST;

MarkonMenu          *markon_menu_new                (GFile        *file) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
MarkonMenu          *markon_menu_new_for_path       (const gchar  *filename) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
MarkonMenu          *markon_menu_new_applications   (void) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
gboolean             markon_menu_load               (MarkonMenu   *menu,
                                                     GCancellable *cancellable,
                                                     GError      **error);
GFile               *markon_menu_get_file           (MarkonMenu   *menu);
MarkonMenuDirectory *markon_menu_get_directory      (MarkonMenu   *menu);
GList               *markon_menu_get_menus          (MarkonMenu   *menu);
void                 markon_menu_add_menu           (MarkonMenu   *menu,
                                                     MarkonMenu   *submenu);
MarkonMenu          *markon_menu_get_menu_with_name (MarkonMenu   *menu,
                                                     const gchar  *name);
MarkonMenu          *markon_menu_get_parent         (MarkonMenu   *menu);
MarkonMenuItemPool  *markon_menu_get_item_pool      (MarkonMenu   *menu);
GList               *markon_menu_get_items          (MarkonMenu   *menu);
GList               *markon_menu_get_elements       (MarkonMenu   *menu);

struct _MarkonMenuClass
{
  GObjectClass __parent__;
};

struct _MarkonMenu
{
  GObject              __parent__;

  /* < private > */
  MarkonMenuPrivate *priv;
};

G_END_DECLS

#endif /* !__MARKON_MENU_H__ */
