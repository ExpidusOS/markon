/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2006-2010 Jannis Pohlmann <jannis@expidus.org>
 * Copyright (c) 2009      Nick Schermer <nick@expidus.org>
 * Copyright (c) 2015      Danila Poyarkov <dannotemail@gmail.com>
 * Copyright (c) 2017      Gregor Santner <gsantner@mailbox.org>
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

#ifndef __MARKON_MENU_ITEM_H__
#define __MARKON_MENU_ITEM_H__

#include <glib-object.h>
#include <gio/gio.h>
#include <markon/markon-menu-item-action.h>

G_BEGIN_DECLS

typedef struct _MarkonMenuItemPrivate MarkonMenuItemPrivate;
typedef struct _MarkonMenuItemClass   MarkonMenuItemClass;
typedef struct _MarkonMenuItem        MarkonMenuItem;

#define MARKON_TYPE_MENU_ITEM            (markon_menu_item_get_type())
#define MARKON_MENU_ITEM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MARKON_TYPE_MENU_ITEM, MarkonMenuItem))
#define MARKON_MENU_ITEM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MARKON_TYPE_MENU_ITEM, MarkonMenuItemClass))
#define MARKON_IS_MENU_ITEM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MARKON_TYPE_MENU_ITEM))
#define MARKON_IS_MENU_ITEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MARKON_TYPE_MENU_ITEM))
#define MARKON_MENU_ITEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MARKON_TYPE_MENU_ITEM, MarkonMenuItemClass))

#define  G_KEY_FILE_DESKTOP_KEY_KEYWORDS "Keywords"

struct _MarkonMenuItemClass
{
  GObjectClass __parent__;

  /* signals */
  void (*changed) (MarkonMenuItem *item);
};

struct _MarkonMenuItem
{
  GObject                  __parent__;

  /* < private > */
  MarkonMenuItemPrivate *priv;
};



GType                 markon_menu_item_get_type                          (void) G_GNUC_CONST;

MarkonMenuItem       *markon_menu_item_new                               (GFile           *file) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
MarkonMenuItem       *markon_menu_item_new_for_path                      (const gchar     *filename) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
MarkonMenuItem       *markon_menu_item_new_for_uri                       (const gchar     *uri) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

gboolean              markon_menu_item_reload                            (MarkonMenuItem  *item,
                                                                    gboolean        *affects_the_outside,
                                                                    GError         **error);

gboolean              markon_menu_item_reload_from_file                  (MarkonMenuItem  *item,
                                                                    GFile           *file,
                                                                    gboolean        *affects_the_outside,
                                                                    GError         **error);

GFile                *markon_menu_item_get_file                          (MarkonMenuItem  *item);

gchar                *markon_menu_item_get_uri                           (MarkonMenuItem  *item) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

const gchar          *markon_menu_item_get_desktop_id                    (MarkonMenuItem  *item);
void                  markon_menu_item_set_desktop_id                    (MarkonMenuItem  *item,
                                                                          const gchar     *desktop_id);

const gchar          *markon_menu_item_get_command                       (MarkonMenuItem  *item);
void                  markon_menu_item_set_command                       (MarkonMenuItem  *item,
                                                                          const gchar     *command);
const gchar          *markon_menu_item_get_try_exec                      (MarkonMenuItem  *item);
void                  markon_menu_item_set_try_exec                      (MarkonMenuItem  *item,
                                                                          const gchar     *try_exec);
const gchar          *markon_menu_item_get_name                          (MarkonMenuItem  *item);
void                  markon_menu_item_set_name                          (MarkonMenuItem  *item,
                                                                          const gchar     *name);
const gchar          *markon_menu_item_get_generic_name                  (MarkonMenuItem  *item);
void                  markon_menu_item_set_generic_name                  (MarkonMenuItem  *item,
                                                                          const gchar     *generic_name);
const gchar          *markon_menu_item_get_comment                       (MarkonMenuItem  *item);
void                  markon_menu_item_set_comment                       (MarkonMenuItem  *item,
                                                                          const gchar     *comment);
const gchar          *markon_menu_item_get_icon_name                     (MarkonMenuItem  *item);
void                  markon_menu_item_set_icon_name                     (MarkonMenuItem  *item,
                                                                          const gchar     *icon_name);
const gchar          *markon_menu_item_get_path                          (MarkonMenuItem  *item);
void                  markon_menu_item_set_path                          (MarkonMenuItem  *item,
                                                                          const gchar     *path);
gboolean              markon_menu_item_get_hidden                        (MarkonMenuItem  *item);
void                  markon_menu_item_set_hidden                        (MarkonMenuItem  *item,
                                                                          gboolean         hidden);
gboolean              markon_menu_item_requires_terminal                 (MarkonMenuItem  *item);
void                  markon_menu_item_set_requires_terminal             (MarkonMenuItem  *item,
                                                                          gboolean         requires_terminal);
gboolean              markon_menu_item_get_no_display                    (MarkonMenuItem  *item);
void                  markon_menu_item_set_no_display                    (MarkonMenuItem  *item,
                                                                          gboolean         no_display);
gboolean              markon_menu_item_supports_startup_notification     (MarkonMenuItem  *item);
void                  markon_menu_item_set_supports_startup_notification (MarkonMenuItem  *item,
                                                                          gboolean         supports_startup_notification);
GList                *markon_menu_item_get_categories                    (MarkonMenuItem  *item);
void                  markon_menu_item_set_categories                    (MarkonMenuItem  *item,
                                                                          GList           *categories);
gboolean              markon_menu_item_has_category                      (MarkonMenuItem  *item,
                                                                          const gchar     *category);
GList                *markon_menu_item_get_keywords                      (MarkonMenuItem  *item);
void                  markon_menu_item_set_keywords                      (MarkonMenuItem  *item,
                                                                          GList           *keywords);
gboolean              markon_menu_item_has_keyword                       (MarkonMenuItem  *item,
                                                                          const gchar     *keyword);
GList                *markon_menu_item_get_actions                       (MarkonMenuItem  *item);
MarkonMenuItemAction *markon_menu_item_get_action                        (MarkonMenuItem  *item,
                                                                          const gchar     *action_name);
void                  markon_menu_item_set_action                        (MarkonMenuItem       *item,
                                                                          const gchar          *action_name,
                                                                          MarkonMenuItemAction *action);
gboolean              markon_menu_item_has_action                        (MarkonMenuItem  *item,
                                                                          const gchar     *action_name);
gboolean              markon_menu_item_get_show_in_environment           (MarkonMenuItem  *item);
gboolean              markon_menu_item_only_show_in_environment          (MarkonMenuItem  *item);
void                  markon_menu_item_ref                               (MarkonMenuItem  *item);
void                  markon_menu_item_unref                             (MarkonMenuItem  *item);
gint                  markon_menu_item_get_allocated                     (MarkonMenuItem  *item);
void                  markon_menu_item_increment_allocated               (MarkonMenuItem  *item);
void                  markon_menu_item_decrement_allocated               (MarkonMenuItem  *item);

G_END_DECLS

#endif /* !__MARKON_MENU_ITEM_H__ */
