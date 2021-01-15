/* $Id$ */
/* vi:set expandtab sw=2 sts=2: */
/*-
 * Copyright (c) 2006-2007 Jannis Pohlmann <jannis@expidus.org>
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

#ifndef __MARKON_MENU_ITEM_CACHE_H__
#define __MARKON_MENU_ITEM_CACHE_H__

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _MarkonMenuItemCachePrivate MarkonMenuItemCachePrivate;
typedef struct _MarkonMenuItemCacheClass   MarkonMenuItemCacheClass;
typedef struct _MarkonMenuItemCache        MarkonMenuItemCache;

#define MARKON_TYPE_MENU_ITEM_CACHE            (markon_menu_item_cache_get_type ())
#define MARKON_MENU_ITEM_CACHE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MARKON_TYPE_MENU_ITEM_CACHE, MarkonMenuItemCache))
#define MARKON_MENU_ITEM_CACHE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MARKON_TYPE_MENU_ITEM_CACHE, MarkonMenuItemCacheClass))
#define MARKON_IS_MENU_ITEM_CACHE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MARKON_TYPE_MENU_ITEM_CACHE))
#define MARKON_IS_MENU_ITEM_CACHE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MARKON_TYPE_MENU_ITEM_CACHE))
#define MARKON_MENU_ITEM_CACHE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MARKON_TYPE_MENU_ITEM_CACHE, MarkonMenuItemCacheClass))

struct _MarkonMenuItemCacheClass
{
  GObjectClass __parent__;
};

struct _MarkonMenuItemCache
{
  GObject                     __parent__;

  /* Private data */
  MarkonMenuItemCachePrivate *priv;
};



GType                markon_menu_item_cache_get_type        (void) G_GNUC_CONST;

MarkonMenuItemCache *markon_menu_item_cache_get_default     (void);

MarkonMenuItem      *markon_menu_item_cache_lookup          (MarkonMenuItemCache *cache,
                                                             const gchar         *uri,
                                                             const gchar         *desktop_id);
void                 markon_menu_item_cache_foreach         (MarkonMenuItemCache *cache,
                                                             GHFunc               func,
                                                             gpointer             user_data);
void                 markon_menu_item_cache_invalidate      (MarkonMenuItemCache *cache);
void                 markon_menu_item_cache_invalidate_file (MarkonMenuItemCache *cache,
                                                             GFile               *file);

G_END_DECLS

#endif /* !__MARKON_MENU_ITEM_CACHE_H__ */
