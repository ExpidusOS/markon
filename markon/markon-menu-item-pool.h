/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2006-2010 Jannis Pohlmann <jannis@expidus.org>
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

#ifndef __MARKON_MENU_ITEM_POOL_H__
#define __MARKON_MENU_ITEM_POOL_H__

#include <glib-object.h>
#include <markon/markon-menu-item.h>

G_BEGIN_DECLS

#define MARKON_TYPE_MENU_ITEM_POOL            (markon_menu_item_pool_get_type ())
#define MARKON_MENU_ITEM_POOL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MARKON_TYPE_MENU_ITEM_POOL, MarkonMenuItemPool))
#define MARKON_MENU_ITEM_POOL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MARKON_TYPE_MENU_ITEM_POOL, MarkonMenuItemPoolClass))
#define MARKON_IS_MENU_ITEM_POOL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MARKON_TYPE_MENU_ITEM_POOL))
#define MARKON_IS_MENU_ITEM_POOL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MARKON_TYPE_MENU_ITEM_POOL))
#define MARKON_MENU_ITEM_POOL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MARKON_TYPE_MENU_ITEM_POOL, MarkonMenuItemPoolClass))

typedef struct _MarkonMenuItemPoolPrivate MarkonMenuItemPoolPrivate;
typedef struct _MarkonMenuItemPoolClass   MarkonMenuItemPoolClass;
typedef struct _MarkonMenuItemPool        MarkonMenuItemPool;

GType               markon_menu_item_pool_get_type           (void) G_GNUC_CONST;

MarkonMenuItemPool *markon_menu_item_pool_new                (void) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

void                markon_menu_item_pool_insert             (MarkonMenuItemPool *pool,
                                                              MarkonMenuItem     *item);
MarkonMenuItem     *markon_menu_item_pool_lookup             (MarkonMenuItemPool *pool,
                                                              const gchar        *desktop_id);
MarkonMenuItem     *markon_menu_item_pool_lookup_file        (MarkonMenuItemPool *pool,
                                                              GFile              *file);
void                markon_menu_item_pool_foreach            (MarkonMenuItemPool *pool,
                                                              GHFunc              func,
                                                              gpointer            user_data);
void                markon_menu_item_pool_apply_exclude_rule (MarkonMenuItemPool *pool,
                                                              GNode              *node);
gboolean            markon_menu_item_pool_get_empty          (MarkonMenuItemPool *pool);
void                markon_menu_item_pool_clear              (MarkonMenuItemPool *pool);

G_END_DECLS

#endif /* !__MARKON_MENU_ITEM_POOL_H__ */
