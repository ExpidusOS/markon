/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2015 Danila Poyarkov <dannotemail@gmail.com>
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

#ifndef __MARKON_MENU_ITEM_ACTION_H__
#define __MARKON_MENU_ITEM_ACTION_H__

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

typedef struct _MarkonMenuItemActionPrivate MarkonMenuItemActionPrivate;
typedef struct _MarkonMenuItemActionClass   MarkonMenuItemActionClass;
typedef struct _MarkonMenuItemAction        MarkonMenuItemAction;

#define MARKON_TYPE_MENU_ITEM_ACTION            (markon_menu_item_action_get_type())
#define MARKON_MENU_ITEM_ACTION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MARKON_TYPE_MENU_ITEM_ACTION, MarkonMenuItemAction))
#define MARKON_MENU_ITEM_ACTION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MARKON_TYPE_MENU_ITEM_ACTION, MarkonMenuItemActionClass))
#define MARKON_IS_MENU_ITEM_ACTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MARKON_TYPE_MENU_ITEM_ACTION))
#define MARKON_IS_MENU_ITEM_ACTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MARKON_TYPE_MENU_ITEM_ACTION))
#define MARKON_MENU_ITEM_ACTION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MARKON_TYPE_MENU_ITEM_ACTION, MarkonMenuItemActionClass))

struct _MarkonMenuItemActionClass
{
  GObjectClass __parent__;

  /* signals */
  void (*changed) (MarkonMenuItemAction *action);
};

struct _MarkonMenuItemAction
{
  GObject                  __parent__;

  /* < private > */
  MarkonMenuItemActionPrivate *priv;
};

GType                 markon_menu_item_action_get_type           (void) G_GNUC_CONST;
MarkonMenuItemAction *markon_menu_item_action_new                (void) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

const gchar          *markon_menu_item_action_get_command        (MarkonMenuItemAction  *action);
void                  markon_menu_item_action_set_command        (MarkonMenuItemAction  *action,
                                                                  const gchar           *command);
const gchar          *markon_menu_item_action_get_name           (MarkonMenuItemAction  *action);
void                  markon_menu_item_action_set_name           (MarkonMenuItemAction  *action,
                                                                  const gchar           *name);
const gchar          *markon_menu_item_action_get_icon_name      (MarkonMenuItemAction  *action);
void                  markon_menu_item_action_set_icon_name      (MarkonMenuItemAction  *action,
                                                                  const gchar           *icon_name);
void                  markon_menu_item_action_ref                (MarkonMenuItemAction  *action);
void                  markon_menu_item_action_unref              (MarkonMenuItemAction  *action);

G_END_DECLS

#endif /* !__MARKON_MENU_ITEM_ACTION_H__ */
