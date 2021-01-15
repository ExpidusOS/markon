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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <markon/markon-menu-node.h>
#include <markon/markon-menu-item-pool.h>



static void     markon_menu_item_pool_finalize       (GObject                 *object);
static gboolean markon_menu_item_pool_filter_exclude (const gchar             *desktop_id,
                                                      MarkonMenuItem          *item,
                                                      GNode                   *node);



struct _MarkonMenuItemPoolClass
{
  GObjectClass __parent__;
};

struct _MarkonMenuItemPoolPrivate
{
  /* Hash table for mapping desktop-file id's to MarkonMenuItem's */
  GHashTable *items;
};

struct _MarkonMenuItemPool
{
  GObject                    __parent__;

  /* < private > */
  MarkonMenuItemPoolPrivate *priv;
};



G_DEFINE_TYPE_WITH_PRIVATE (MarkonMenuItemPool, markon_menu_item_pool, G_TYPE_OBJECT)



static void
markon_menu_item_pool_class_init (MarkonMenuItemPoolClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = markon_menu_item_pool_finalize;
}



static void
markon_menu_item_pool_init (MarkonMenuItemPool *pool)
{
  pool->priv = markon_menu_item_pool_get_instance_private (pool);
  pool->priv->items = g_hash_table_new_full (g_str_hash, g_str_equal,
                                             g_free,
                                             (GDestroyNotify) markon_menu_item_unref);
}



static void
markon_menu_item_pool_finalize (GObject *object)
{
  MarkonMenuItemPool *pool = MARKON_MENU_ITEM_POOL (object);

  g_hash_table_unref (pool->priv->items);

  (*G_OBJECT_CLASS (markon_menu_item_pool_parent_class)->finalize) (object);
}


/**
 * markon_menu_item_pool_new: (constructor)
 *
 * Returns: (transfer full): a #MarkonMenuItemPool
 */
MarkonMenuItemPool*
markon_menu_item_pool_new (void)
{
  return g_object_new (MARKON_TYPE_MENU_ITEM_POOL, NULL);
}


/**
 * markon_menu_item_pool_insert:
 * @pool: a #MarkonMenuItemPool
 * @item: a #MarkonMenuItem
 */
void
markon_menu_item_pool_insert (MarkonMenuItemPool *pool,
                              MarkonMenuItem     *item)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM_POOL (pool));
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));

  /* Insert into the hash table and remove old item (if any) */
  g_hash_table_replace (pool->priv->items, g_strdup (markon_menu_item_get_desktop_id (item)), item);

  /* Grab a reference on the item */
  markon_menu_item_ref (item);
}


/**
 * markon_menu_item_pool_lookup:
 * @pool: a #MarkonMenuItemPool
 * @desktop_id: (type filename): .desktop file
 *
 * Returns: (transfer full): a #MarkonMenuItem object
 */
MarkonMenuItem*
markon_menu_item_pool_lookup (MarkonMenuItemPool *pool,
                              const gchar        *desktop_id)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM_POOL (pool), NULL);
  g_return_val_if_fail (desktop_id != NULL, NULL);

  return g_hash_table_lookup (pool->priv->items, desktop_id);
}


/**
 * markon_menu_item_pool_lookup_file:
 * @pool: a #MarkonMenuItemPool
 * @file: a GFile instance
 *
 * Returns: (transfer full): a #MarkonMenuItem object
 */
MarkonMenuItem *
markon_menu_item_pool_lookup_file (MarkonMenuItemPool *pool,
                                   GFile              *file)
{
  MarkonMenuItem *result = NULL;
  GHashTableIter  iter;
  gpointer        item;
  GFile          *item_file;

  g_return_val_if_fail (MARKON_IS_MENU_ITEM_POOL (pool), NULL);
  g_return_val_if_fail (G_IS_FILE (file), NULL);

  g_hash_table_iter_init (&iter, pool->priv->items);
  while (result == NULL && g_hash_table_iter_next (&iter, NULL, &item))
    {
      item_file = markon_menu_item_get_file (item);

      if (g_file_equal (item_file, file))
        result = item;

      g_object_unref (item_file);
    }

  return result;
}


/**
 * markon_menu_item_pool_foreach:
 * @pool: a #MarkonMenuItemPool
 * @func: (scope call):
 * @user_data: user data passed to @func callback
 */
void
markon_menu_item_pool_foreach (MarkonMenuItemPool *pool,
                               GHFunc              func,
                               gpointer            user_data)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM_POOL (pool));

  g_hash_table_foreach (pool->priv->items, func, user_data);
}



void
markon_menu_item_pool_apply_exclude_rule (MarkonMenuItemPool *pool,
                                          GNode              *node)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM_POOL (pool));
  g_return_if_fail (node != NULL);

  /* Remove all items which match this exclude rule */
  g_hash_table_foreach_remove (pool->priv->items, (GHRFunc) markon_menu_item_pool_filter_exclude, node);
}



static gboolean
markon_menu_item_pool_filter_exclude (const gchar    *desktop_id,
                                      MarkonMenuItem *item,
                                      GNode          *node)
{
  gboolean matches;

  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), FALSE);
  g_return_val_if_fail (node != NULL, FALSE);

  matches = markon_menu_node_tree_rule_matches (node, item);

  if (matches)
    markon_menu_item_increment_allocated (item);

  return matches;
}



gboolean
markon_menu_item_pool_get_empty (MarkonMenuItemPool *pool)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM_POOL (pool), TRUE);
  return (g_hash_table_size (pool->priv->items) == 0);
}



void
markon_menu_item_pool_clear (MarkonMenuItemPool *pool)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM_POOL (pool));
  g_hash_table_remove_all (pool->priv->items);
}
