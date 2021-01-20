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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <markon/markon-menu-item.h>
#include <markon/markon-menu-item-cache.h>



static void markon_menu_item_cache_finalize   (GObject                  *object);



/* Object Mutex Lock */
#define _item_cache_lock(cache)    g_mutex_lock (&((cache)->priv->lock))
#define _item_cache_unlock(cache)  g_mutex_unlock (&((cache)->priv->lock))



struct _MarkonMenuItemCachePrivate
{
  /* Hash table for mapping absolute filenames to MarkonMenuItem's */
  GHashTable *items;

  GMutex      lock;
};



G_DEFINE_TYPE_WITH_PRIVATE (MarkonMenuItemCache, markon_menu_item_cache, G_TYPE_OBJECT)



static void
markon_menu_item_cache_class_init (MarkonMenuItemCacheClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = markon_menu_item_cache_finalize;
}



static void
markon_menu_item_cache_init (MarkonMenuItemCache *cache)
{
  cache->priv = markon_menu_item_cache_get_instance_private (cache);

  g_mutex_init (&cache->priv->lock);

  /* Create empty hash table */
  cache->priv->items = g_hash_table_new_full (g_str_hash, g_str_equal,
                                              g_free,
                                              (GDestroyNotify) markon_menu_item_unref);
}



/**
 * markon_menu_item_cache_get_default: (constructor)
 *
 * Returns the default #MarkonMenuItemCache. the returned object
 * should be unreffed with g_object_unref() when no longer needed.
 *
 * Returns: (transfer full): a new #MarkonMenuItemCache.
 */
MarkonMenuItemCache*
markon_menu_item_cache_get_default (void)
{
  static MarkonMenuItemCache *cache = NULL;

  if (G_UNLIKELY (cache == NULL))
    {
      /* Create a new cache */
      cache = g_object_new (MARKON_TYPE_MENU_ITEM_CACHE, NULL);
      g_object_add_weak_pointer (G_OBJECT (cache), (gpointer) &cache);
    }
  else
    {
      /* Take a reference */
      g_object_ref (G_OBJECT (cache));
    }

  return cache;
}



static void
markon_menu_item_cache_finalize (GObject *object)
{
  MarkonMenuItemCache *cache = MARKON_MENU_ITEM_CACHE (object);

  /* Free hash table */
  g_hash_table_unref (cache->priv->items);

  /*Release the mutex */
  g_mutex_clear (&cache->priv->lock);

  (*G_OBJECT_CLASS (markon_menu_item_cache_parent_class)->finalize) (object);
}


/**
 * markon_menu_item_cache_lookup:
 * @cache: a #MarkonMenuItemCache
 * @uri:
 * @desktop_id
 *
 * Returns: (transfer full) (nullable): a #MarkonMenuItem
 */
MarkonMenuItem*
markon_menu_item_cache_lookup (MarkonMenuItemCache *cache,
                               const gchar         *uri,
                               const gchar         *desktop_id)
{
  MarkonMenuItem *item = NULL;

  g_return_val_if_fail (MARKON_IS_MENU_ITEM_CACHE (cache), NULL);
  g_return_val_if_fail (uri != NULL, NULL);
  g_return_val_if_fail (desktop_id != NULL, NULL);

  /* Acquire lock on the item cache as it's likely that we need to load
   * items from the hard drive and store it in the hash table of the
   * item cache */
  _item_cache_lock (cache);

  /* Search uri in the hash table */
  item = g_hash_table_lookup (cache->priv->items, uri);

  /* Return the item if we we found one */
  if (item != NULL)
    {
      /* Update desktop id, if necessary */
      markon_menu_item_set_desktop_id (item, desktop_id);

      /* Release item cache lock */
      _item_cache_unlock (cache);

      return item;
    }

  /* Last chance is to load it directly from the file */
  item = markon_menu_item_new_for_uri (uri);

  if (G_LIKELY (item != NULL))
    {
      /* Update desktop id */
      markon_menu_item_set_desktop_id (item, desktop_id);

      /* The file has been loaded, add the item to the hash table */
      g_hash_table_replace (cache->priv->items, g_strdup (uri), item);
    }

  /* Release item cache lock */
  _item_cache_unlock (cache);

  return item;
}


/**
 * markon_menu_item_cache_foreach:
 * @cache: a #MarkonMenuItemCache
 * @func: (scope call):
 * @user_data: parameter passed to @func callback
 */
void
markon_menu_item_cache_foreach (MarkonMenuItemCache *cache,
                                GHFunc               func,
                                gpointer             user_data)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM_CACHE (cache));

  /* Acquire lock on the item cache */
  _item_cache_lock (cache);

  g_hash_table_foreach (cache->priv->items, func, user_data);

  /* Release item cache lock */
  _item_cache_unlock (cache);
}



void
markon_menu_item_cache_invalidate (MarkonMenuItemCache *cache)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM_CACHE (cache));

  /* Acquire lock on the item cache */
  _item_cache_lock (cache);

  /* Remove all items from the hash table */
  g_hash_table_remove_all (cache->priv->items);

  /* Release item cache lock */
  _item_cache_unlock (cache);
}



void
markon_menu_item_cache_invalidate_file (MarkonMenuItemCache *cache,
                                        GFile               *file)
{
  gchar *uri;

  g_return_if_fail (MARKON_IS_MENU_ITEM_CACHE (cache));
  g_return_if_fail (G_IS_FILE (file));

  uri = g_file_get_uri (file);

  /* Acquire a lock on the item cache */
  _item_cache_lock (cache);

  /* Remove possible items with this URI from the cache */
  g_hash_table_remove (cache->priv->items, uri);

  /* Release the item cache lock */
  _item_cache_unlock (cache);

  g_free (uri);
}