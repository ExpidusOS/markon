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

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <libexpidus1util/libexpidus1util.h>

#include <markon/markon-config.h>
#include <markon/markon-environment.h>
#include <markon/markon-marshal.h>
#include <markon/markon-menu-element.h>
#include <markon/markon-menu-item.h>
#include <markon/markon-menu-directory.h>
#include <markon/markon-menu-item-cache.h>
#include <markon/markon-menu-separator.h>
#include <markon/markon-menu-node.h>
#include <markon/markon-menu-parser.h>
#include <markon/markon-menu-merger.h>
#include <markon/markon-private.h>



/* Use g_access() on win32 */
#if defined(G_OS_WIN32)
#include <glib/gstdio.h>
#else
#define g_access(filename, mode) (access ((filename), (mode)))
#endif



/**
 * SECTION: markon-menu
 * @title: MarkonMenu
 * @short_description: Menu element.
 * @include: markon/markon.h
 *
 * Main element the contains the parsed menu.
 **/



typedef struct _MarkonMenuPair
{
  gpointer first;
  gpointer second;
} MarkonMenuPair;



/* Property identifiers */
enum
{
  PROP_0,
  PROP_ENVIRONMENT,
  PROP_FILE,
  PROP_DIRECTORY,
  PROP_PARENT, /* TODO */
};



/* Signal identifiers */
enum
{
  RELOAD_REQUIRED,
  DIRECTORY_CHANGED,
  LAST_SIGNAL
};



static void                 markon_menu_element_init                    (MarkonMenuElementIface  *iface);
static void                 markon_menu_clear                           (MarkonMenu              *menu);
static void                 markon_menu_finalize                        (GObject                 *object);
static void                 markon_menu_get_property                    (GObject                 *object,
                                                                         guint                    prop_id,
                                                                         GValue                  *value,
                                                                         GParamSpec              *pspec);
static void                 markon_menu_set_property                    (GObject                 *object,
                                                                         guint                    prop_id,
                                                                         const GValue            *value,
                                                                         GParamSpec              *pspec);
static void                 markon_menu_set_directory                   (MarkonMenu              *menu,
                                                                         MarkonMenuDirectory     *directory);
static void                 markon_menu_resolve_menus                   (MarkonMenu              *menu);
static void                 markon_menu_resolve_directory               (MarkonMenu              *menu,
                                                                         GCancellable            *cancellable,
                                                                         gboolean                 recursive);
static MarkonMenuDirectory *markon_menu_lookup_directory                (MarkonMenu              *menu,
                                                                         const gchar             *filename);
static void                 markon_menu_collect_files                   (MarkonMenu              *menu,
                                                                         GHashTable              *desktop_id_table);
static void                 markon_menu_collect_files_from_path         (MarkonMenu              *menu,
                                                                         GHashTable              *desktop_id_table,
                                                                         GFile                   *path,
                                                                         const gchar             *id_prefix);
static void                 markon_menu_resolve_items                   (MarkonMenu              *menu,
                                                                         GHashTable              *desktop_id_table,
                                                                         gboolean                 only_unallocated);
static void                 markon_menu_resolve_items_by_rule           (MarkonMenu              *menu,
                                                                         GHashTable              *desktop_id_table,
                                                                         GNode                   *node);
static void                 markon_menu_resolve_item_by_rule            (const gchar             *desktop_id,
                                                                         const gchar             *uri,
                                                                         MarkonMenuPair          *data);
static void                 markon_menu_remove_deleted_menus            (MarkonMenu              *menu);
static gint                 markon_menu_compare_items                   (gconstpointer           *a,
                                                                         gconstpointer           *b);
static const gchar         *markon_menu_get_element_name                (MarkonMenuElement       *element);
static const gchar         *markon_menu_get_element_comment             (MarkonMenuElement       *element);
static const gchar         *markon_menu_get_element_icon_name           (MarkonMenuElement       *element);
static gboolean             markon_menu_get_element_visible             (MarkonMenuElement       *element);
static gboolean             markon_menu_get_element_show_in_environment (MarkonMenuElement       *element);
static gboolean             markon_menu_get_element_no_display          (MarkonMenuElement       *element);
static gboolean             markon_menu_get_element_equal               (MarkonMenuElement       *element,
                                                                         MarkonMenuElement       *other);
static void                 markon_menu_start_monitoring                (MarkonMenu              *menu);
static void                 markon_menu_stop_monitoring                 (MarkonMenu              *menu);
static void                 markon_menu_monitor_menu_files              (MarkonMenu              *menu);
static void                 markon_menu_monitor_files                   (MarkonMenu              *menu,
                                                                         GList                   *files,
                                                                         gpointer                 callback);
static void                 markon_menu_monitor_app_dirs                (MarkonMenu              *menu);
static void                 markon_menu_monitor_directory_dirs          (MarkonMenu              *menu);
static void                 markon_menu_file_changed                    (MarkonMenu              *menu,
                                                                         GFile                   *file,
                                                                         GFile                   *other_file,
                                                                         GFileMonitorEvent        event_type,
                                                                         GFileMonitor            *monitor);
static void                 markon_menu_merge_file_changed              (MarkonMenu              *menu,
                                                                         GFile                   *file,
                                                                         GFile                   *other_file,
                                                                         GFileMonitorEvent        event_type,
                                                                         GFileMonitor            *monitor);
static void                 markon_menu_merge_dir_changed               (MarkonMenu              *menu,
                                                                         GFile                   *file,
                                                                         GFile                   *other_file,
                                                                         GFileMonitorEvent        event_type,
                                                                         GFileMonitor            *monitor);
static void                 markon_menu_app_dir_changed                 (MarkonMenu              *menu,
                                                                         GFile                   *file,
                                                                         GFile                   *other_file,
                                                                         GFileMonitorEvent        event_type,
                                                                         GFileMonitor            *monitor);
static void                 markon_menu_directory_file_changed          (MarkonMenu              *menu,
                                                                         GFile                   *file,
                                                                         GFile                   *other_file,
                                                                         GFileMonitorEvent        event_type,
                                                                         GFileMonitor            *monitor);
static MarkonMenuItem      *markon_menu_find_file_item                  (MarkonMenu              *menu,
                                                                         GFile                   *file);



struct _MarkonMenuPrivate
{
  /* Menu file */
  GFile               *file;

  /* DOM tree */
  GNode               *tree;

  /* Merged menu files and merge directories */
  GList               *merge_files;
  GList               *merge_dirs;

  /* File and directory monitors */
  GList               *monitors;

  /* Directory */
  MarkonMenuDirectory *directory;

  /* Submenus */
  GList               *submenus;

  /* Parent menu */
  MarkonMenu          *parent;

  /* Menu item pool */
  MarkonMenuItemPool  *pool;

  /* Shared menu item cache */
  MarkonMenuItemCache *cache;

  /* List to merge consecutive file changes into a a single event */
  GSList              *changed_files;
  guint                file_changed_idle;

  /* Flag for marking custom path menus */
  guint                uses_custom_path : 1;

  /* idle reload-required to group events */
  guint                idle_reload_required_id;
};



G_DEFINE_TYPE_WITH_CODE (MarkonMenu, markon_menu, G_TYPE_OBJECT,
                         G_ADD_PRIVATE (MarkonMenu)
                         G_IMPLEMENT_INTERFACE (MARKON_TYPE_MENU_ELEMENT,
                                                markon_menu_element_init))



static guint menu_signals[LAST_SIGNAL];
static GQuark markon_menu_file_quark;



static void
markon_menu_class_init (MarkonMenuClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = markon_menu_finalize;
  gobject_class->get_property = markon_menu_get_property;
  gobject_class->set_property = markon_menu_set_property;

  /**
   * MarkonMenu:file:
   *
   * The #GFile from which the %MarkonMenu was loaded.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_FILE,
                                   g_param_spec_object ("file",
                                                        "file",
                                                        "file",
                                                        G_TYPE_FILE,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS |
                                                        G_PARAM_CONSTRUCT_ONLY));

  /**
   * MarkonMenu:directory:
   *
   * The directory entry associated with this menu.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_DIRECTORY,
                                   g_param_spec_object ("directory",
                                                        "Directory",
                                                        "Directory entry associated with this menu",
                                                        MARKON_TYPE_MENU_DIRECTORY,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

  menu_signals[RELOAD_REQUIRED] =
    g_signal_new ("reload-required",
                  MARKON_TYPE_MENU,
                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS,
                  0,
                  NULL,
                  NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE,
                  0);

  menu_signals[DIRECTORY_CHANGED] =
    g_signal_new ("directory-changed",
                  MARKON_TYPE_MENU,
                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_HOOKS,
                  0,
                  NULL,
                  NULL,
                  markon_marshal_VOID__OBJECT_OBJECT,
                  G_TYPE_NONE,
                  2,
                  MARKON_TYPE_MENU_DIRECTORY,
                  MARKON_TYPE_MENU_DIRECTORY);

  markon_menu_file_quark = g_quark_from_string ("markon-menu-file-quark");
}



static void
markon_menu_element_init (MarkonMenuElementIface *iface)
{
  iface->get_name = markon_menu_get_element_name;
  iface->get_comment = markon_menu_get_element_comment;
  iface->get_icon_name = markon_menu_get_element_icon_name;
  iface->get_visible = markon_menu_get_element_visible;
  iface->get_show_in_environment = markon_menu_get_element_show_in_environment;
  iface->get_no_display = markon_menu_get_element_no_display;
  iface->equal = markon_menu_get_element_equal;
}



static void
markon_menu_init (MarkonMenu *menu)
{
  menu->priv = markon_menu_get_instance_private (menu);
  menu->priv->file = NULL;
  menu->priv->tree = NULL;
  menu->priv->merge_files = NULL;
  menu->priv->merge_dirs = NULL;
  menu->priv->monitors = NULL;
  menu->priv->directory = NULL;
  menu->priv->submenus = NULL;
  menu->priv->parent = NULL;
  menu->priv->pool = markon_menu_item_pool_new ();
  menu->priv->uses_custom_path = TRUE;
  menu->priv->changed_files = NULL;
  menu->priv->idle_reload_required_id = 0;

  /* Take reference on the menu item cache */
  menu->priv->cache = markon_menu_item_cache_get_default ();
}



static void
markon_menu_clear (MarkonMenu *menu)
{
  g_return_if_fail (MARKON_IS_MENU (menu));

  /* Check if the menu is the root menu */
  if (menu->priv->parent == NULL)
    {
      /* Stop monitoring recursively */
      markon_menu_stop_monitoring (menu);

      /* Destroy the menu tree */
      markon_menu_node_tree_free (menu->priv->tree);
      menu->priv->tree = NULL;

      /* Release the merge files */
      _markon_g_list_free_full (menu->priv->merge_files, g_object_unref);
      menu->priv->merge_files = NULL;

      /* Release the merge dirs */
      _markon_g_list_free_full (menu->priv->merge_dirs, g_object_unref);
      menu->priv->merge_dirs = NULL;
    }

  /* Free submenus */
  _markon_g_list_free_full (menu->priv->submenus, g_object_unref);
  menu->priv->submenus = NULL;

  /* Free directory */
  if (G_LIKELY (menu->priv->directory != NULL))
    {
      g_object_unref (menu->priv->directory);
      menu->priv->directory = NULL;
    }

  /* Clear the item pool */
  markon_menu_item_pool_clear (menu->priv->pool);

  /* Stop reload-required emit */
  if (menu->priv->idle_reload_required_id != 0)
    {
      g_source_remove (menu->priv->idle_reload_required_id);
      menu->priv->idle_reload_required_id = 0;
    }
}



static void
markon_menu_finalize (GObject *object)
{
  MarkonMenu *menu = MARKON_MENU (object);

  /* Clear resources allocated in the load process */
  markon_menu_clear (menu);

  /* Free file */
  if (menu->priv->file != NULL)
    g_object_unref (menu->priv->file);

  /* Free item pool */
  g_object_unref (menu->priv->pool);

  /* Release item cache reference */
  g_object_unref (menu->priv->cache);

  (*G_OBJECT_CLASS (markon_menu_parent_class)->finalize) (object);
}



static void
markon_menu_get_property (GObject    *object,
                          guint       prop_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  MarkonMenu *menu = MARKON_MENU (object);

  switch (prop_id)
    {
    case PROP_FILE:
      g_value_set_object (value, menu->priv->file);
      break;

    case PROP_DIRECTORY:
      g_value_set_object (value, menu->priv->directory);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
markon_menu_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  MarkonMenu *menu = MARKON_MENU (object);

  switch (prop_id)
    {
    case PROP_FILE:
      menu->priv->file = g_value_dup_object (value);
      break;

    case PROP_DIRECTORY:
      markon_menu_set_directory (menu, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



/**
 * markon_menu_new: (constructor)
 * @file: #GFile for the .menu file you want to load.
 *
 * Creates a new #MarkonMenu for the .menu file referred to by @file.
 * This operation only fails @file is invalid. To load the menu
 * tree from the file, you need to call markon_menu_load() with the
 * returned #MarkonMenu.
 *
 * The caller is responsible to destroy the returned #MarkonMenu
 * using g_object_unref().
 *
 * For more information about the usage @see markon_menu_new().
 *
 * Returns: (transfer full): a new #MarkonMenu.
 **/
MarkonMenu *
markon_menu_new (GFile *file)
{
  g_return_val_if_fail (G_IS_FILE (file), NULL);
  return g_object_new (MARKON_TYPE_MENU, "file", file, NULL);
}



/**
 * markon_menu_new_for_path: (constructor)
 * @filename : Path/URI of the .menu file you want to load.
 *
 * Creates a new #MarkonMenu for the .menu file referred to by @filename.
 * This operation only fails if the filename is NULL. To load the menu
 * tree from the file, you need to call markon_menu_load() with the
 * returned #MarkonMenu.
 *
 * <informalexample><programlisting>
 * MarkonMenu *menu = markon_menu_new (filename);
 *
 * if (markon_menu_load (menu, &error))
 *   ...
 * else
 *   ...
 *
 * g_object_unref (menu);
 * </programlisting></informalexample>
 *
 * The caller is responsible to destroy the returned #MarkonMenu
 * using g_object_unref().
 *
 * Returns: (transfer full): a new #MarkonMenu
 * for @filename.
 **/
MarkonMenu *
markon_menu_new_for_path (const gchar *filename)
{
  MarkonMenu *menu;
  GFile      *file;

  g_return_val_if_fail (filename != NULL, NULL);

  /* Create new menu */
  file = _markon_file_new_for_unknown_input (filename, NULL);
  menu = g_object_new (MARKON_TYPE_MENU, "file", file, NULL);
  g_object_unref (file);

  return menu;
}



/**
 * markon_menu_new_applications:
 *
 * Creates a new #MarkonMenu for the applications.menu file
 * which is being used to display installed applications.
 *
 * For more information about the usage @see markon_menu_new().
 *
 * Returns: (transfer full): a new #MarkonMenu
 * for applications.menu.
 **/
MarkonMenu *
markon_menu_new_applications (void)
{
  MarkonMenu *menu = NULL;

  menu = g_object_new (MARKON_TYPE_MENU, NULL);
  menu->priv->uses_custom_path = FALSE;

  return menu;
}



/**
 * markon_menu_get_file:
 * @menu: a #MarkonMenu.
 *
 * Get the file for @menu. It refers to the .menu file from which
 * @menu was or will be loaded.
 *
 * The returned object should be unreffed with g_object_unref()
 * when no longer needed.
 *
 * Returns: (transfer full): a #GFile.
 */
GFile *
markon_menu_get_file (MarkonMenu *menu)
{
  g_return_val_if_fail (MARKON_IS_MENU (menu), NULL);
  return g_object_ref (menu->priv->file);
}




static const gchar *
markon_menu_get_name (MarkonMenu *menu)
{
  g_return_val_if_fail (MARKON_IS_MENU (menu), NULL);
  return markon_menu_node_tree_get_string_child (menu->priv->tree,
                                                 MARKON_MENU_NODE_TYPE_NAME);
}



/**
 * markon_menu_get_directory:
 * @menu: a #MarkonMenu.
 *
 * Returns the #MarkonMenuDirectory of @menu or %NULL if @menu has
 * no valid directory element.
 *
 * The returned object should be unreffed with g_object_unref() when
 * no longer needed.
 *
 * The menu directory may contain a lot of useful information about
 * the menu like the display and icon name, desktop environments it
 * should show up in etc.
 *
 * Returns: (transfer full) (nullable): a #MarkonMenuDirectory
 */
MarkonMenuDirectory*
markon_menu_get_directory (MarkonMenu *menu)
{
  g_return_val_if_fail (MARKON_IS_MENU (menu), NULL);
  return menu->priv->directory;
}



static void
markon_menu_set_directory (MarkonMenu          *menu,
                           MarkonMenuDirectory *directory)
{
  g_return_if_fail (MARKON_IS_MENU (menu));
  g_return_if_fail (MARKON_IS_MENU_DIRECTORY (directory));

  /* Abort if directories are equal */
  if (markon_menu_directory_equal (directory, menu->priv->directory))
    return;

  /* Destroy old directory */
  if (menu->priv->directory != NULL)
    g_object_unref (menu->priv->directory);

  /* Remove the floating reference and acquire a normal one */
  g_object_ref_sink (directory);

  /* Set the new directory */
  menu->priv->directory = directory;

  /* Notify listeners */
  g_object_notify (G_OBJECT (menu), "directory");
}



/**
 * markon_menu_load:
 * @menu        : a #MarkonMenu
 * @cancellable : a #GCancellable
 * @error       : #GError return location
 *
 * This function loads the entire menu tree from the file referred to
 * by @menu. It resolves merges, moves and everything else defined
 * in the menu specification. The resulting tree information is
 * stored within @menu and can be accessed using the public #MarkonMenu
 * API afterwards.
 *
 * @cancellable can be used to handle blocking I/O when reading data
 * from files during the loading process.
 *
 * @error should either be NULL or point to a #GError return location
 * where errors should be stored in.
 *
 * Returns: %TRUE if the menu was loaded successfully or
 *          %FALSE if there was an error or the process was
 *          cancelled.
 **/
gboolean
markon_menu_load (MarkonMenu   *menu,
                  GCancellable *cancellable,
                  GError      **error)
{
  MarkonMenuParser *parser;
  MarkonMenuMerger *merger;
  GHashTable       *desktop_id_table;
  const gchar      *prefix;
  gboolean          success = TRUE;
  gchar            *filename;
  gchar            *relative_filename;

  g_return_val_if_fail (MARKON_IS_MENU (menu), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  /* Make sure to reset the menu to a loadable state */
  markon_menu_clear (menu);

  /* Check if we need to locate the applications menu file */
  if (!menu->priv->uses_custom_path)
    {
      /* Release the old file if there is one */
      if (menu->priv->file != NULL)
        {
          g_object_unref (menu->priv->file);
          menu->priv->file = NULL;
        }

      /* Build the ${XDG_MENU_PREFIX}applications.menu filename */
      prefix = g_getenv ("XDG_MENU_PREFIX");
      relative_filename = g_strconcat ("menus", G_DIR_SEPARATOR_S,
                                       prefix != NULL ? prefix : MARKON_DEFAULT_MENU_PREFIX,
                                       "applications.menu", NULL);

      /* Search for the menu file in user and system config dirs */
      filename = markon_config_lookup (relative_filename);

      /* Use the file if it exists */
      if (filename != NULL)
        menu->priv->file = _markon_file_new_for_unknown_input (filename, NULL);

      /* Abort with an error if no suitable applications menu file was found */
      if (menu->priv->file == NULL)
        {
          g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_NOENT,
                       _("File \"%s\" not found"), relative_filename);
          g_free (relative_filename);
          return FALSE;
        }

      /* Free the filename strings */
      g_free (relative_filename);
      g_free (filename);

    }

  parser = markon_menu_parser_new (menu->priv->file);

  if (markon_menu_parser_run (parser, cancellable, error))
    {
      merger = markon_menu_merger_new (MARKON_MENU_TREE_PROVIDER (parser));

      if (markon_menu_merger_run (merger,
                                  &menu->priv->merge_files,
                                  &menu->priv->merge_dirs,
                                  cancellable, error))
        {
          menu->priv->tree =
            markon_menu_tree_provider_get_tree (MARKON_MENU_TREE_PROVIDER (merger));
        }
      else
        {
          success = FALSE;
        }

      g_object_unref (merger);
    }
  else
    success = FALSE;

  g_object_unref (parser);

  if (!success)
    return FALSE;

  /* Generate submenus */
  markon_menu_resolve_menus (menu);

  /* Resolve the menu directory */
  markon_menu_resolve_directory (menu, cancellable, TRUE);

  /* Abort if the cancellable was cancelled */
  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return FALSE;

  desktop_id_table = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

  /* Load menu items */
  markon_menu_collect_files (menu, desktop_id_table);
  markon_menu_resolve_items (menu, desktop_id_table, FALSE);
  markon_menu_resolve_items (menu, desktop_id_table, TRUE);

  /* Remove deleted menus */
  markon_menu_remove_deleted_menus (menu);

  g_hash_table_unref (desktop_id_table);

  /* Initiate file system monitoring */
  markon_menu_start_monitoring (menu);

  return TRUE;
}



/**
 * markon_menu_get_menus:
 * @menu: a #MarkonMenu
 *
 * Returns a sorted list of #MarkonMenu submenus of @menu. The list
 * should be freed with g_list_free().
 *
 * Returns: (transfer full) (element-type MarkonMenu): a sorted list
 * of #MarkonMenu.
 */
GList *
markon_menu_get_menus (MarkonMenu *menu)
{
  GList *menus = NULL;

  g_return_val_if_fail (MARKON_IS_MENU (menu), NULL);

  /* Copy submenu list */
  menus = g_list_copy (menu->priv->submenus);

  /* Sort submenus */
  menus = g_list_sort (menus, (GCompareFunc) markon_menu_compare_items);

  return menus;
}



/**
 * markon_menu_add_menu:
 * @menu:    a #MarkonMenu
 * @submenu: a #MarkonMenu
 *
 * Adds @submenu as a sub menu to @menu.
 **/
void
markon_menu_add_menu (MarkonMenu *menu,
                      MarkonMenu *submenu)
{
  g_return_if_fail (MARKON_IS_MENU (menu));
  g_return_if_fail (MARKON_IS_MENU (submenu));

  /* Remove floating reference and acquire a 'real' one */
  g_object_ref_sink (G_OBJECT (submenu));

  /* Append menu to the list */
  menu->priv->submenus = g_list_append (menu->priv->submenus, submenu);

  /* TODO: Use property method here */
  submenu->priv->parent = menu;
}



/**
 * markon_menu_get_menu_with_name:
 * @menu: a #MarkonMenu
 * @name: a sub menu name
 *
 * Looks in @menu for a submenu with @name as name.
 *
 * Returns: (transfer full) (nullable): a #MarkonMenu or %NULL.
 */
MarkonMenu *
markon_menu_get_menu_with_name (MarkonMenu  *menu,
                                const gchar *name)
{
  MarkonMenu *result = NULL;
  GList      *iter;

  g_return_val_if_fail (MARKON_IS_MENU (menu), NULL);
  g_return_val_if_fail (name != NULL, NULL);

  /* Iterate over the submenu list */
  for (iter = menu->priv->submenus; result == NULL && iter != NULL; iter = g_list_next (iter))
    {
      /* End loop when a matching submenu is found */
      if (G_UNLIKELY (g_strcmp0 (markon_menu_get_name (iter->data), name) == 0))
        result = iter->data;
    }

  return result;
}



/**
 * markon_menu_get_parent:
 * @menu: a #MarkonMenu
 *
 * Returns the parent #MarkonMenu or @menu.
 *
 * Returns: (transfer full) (nullable): a #MarkonMenu or %NULL
 * if @menu is the root menu.
 */
MarkonMenu *
markon_menu_get_parent (MarkonMenu *menu)
{
  g_return_val_if_fail (MARKON_IS_MENU (menu), NULL);
  return menu->priv->parent;
}



static void
markon_menu_resolve_menus (MarkonMenu *menu)
{
  MarkonMenu *submenu;
  GList      *menus = NULL;
  GList      *iter;

  g_return_if_fail (MARKON_IS_MENU (menu));

  menus = markon_menu_node_tree_get_child_nodes (menu->priv->tree,
                                                 MARKON_MENU_NODE_TYPE_MENU,
                                                 FALSE);

  for (iter = menus; iter != NULL; iter = g_list_next (iter))
    {
      submenu = g_object_new (MARKON_TYPE_MENU, "file", menu->priv->file, NULL);
      submenu->priv->tree = iter->data;
      markon_menu_add_menu (menu, submenu);
      g_object_unref (submenu);
    }

  g_list_free (menus);

  for (iter = menu->priv->submenus; iter != NULL; iter = g_list_next (iter))
    markon_menu_resolve_menus (iter->data);
}



static GList *
markon_menu_get_directories (MarkonMenu *menu)
{
  GList *dirs = NULL;

  /* Fetch all application directories */
  dirs = markon_menu_node_tree_get_string_children (menu->priv->tree,
                                                    MARKON_MENU_NODE_TYPE_DIRECTORY,
                                                    TRUE);

  if (menu->priv->parent != NULL)
    dirs = g_list_concat (dirs, markon_menu_get_directories (menu->priv->parent));

  return dirs;
}



static void
markon_menu_resolve_directory (MarkonMenu   *menu,
                               GCancellable *cancellable,
                               gboolean      recursive)
{
  MarkonMenuDirectory *directory = NULL;
  GList               *directories = NULL;
  GList               *iter;

  g_return_if_fail (MARKON_IS_MENU (menu));
  g_return_if_fail (cancellable == NULL || G_IS_CANCELLABLE (cancellable));

  /* release the old directory if there is one */
  if (menu->priv->directory != NULL)
    {
      g_object_unref (menu->priv->directory);
      menu->priv->directory = NULL;
    }

  /* Determine all directories for this menu */
  directories = markon_menu_get_directories (menu);

  /* Try to load one directory name after another */
  for (iter = directories; directory == NULL && iter != NULL; iter = g_list_next (iter))
    {
      /* Try to load the directory with this name */
      directory = markon_menu_lookup_directory (menu, iter->data);
    }

  if (G_LIKELY (directory != NULL))
    {
      /* Set the directory (assuming that we found at least one valid name) */
      menu->priv->directory = directory;
    }

  /* Free reverse list copy */
  g_list_free (directories);

  if (recursive)
    {
      /* Resolve directories of submenus recursively */
      for (iter = menu->priv->submenus; iter != NULL; iter = g_list_next (iter))
        markon_menu_resolve_directory (iter->data, cancellable, recursive);
    }
}



static GList *
markon_menu_get_directory_dirs (MarkonMenu *menu)
{
  GList *dirs = NULL;

  /* Fetch all application directories */
  dirs = markon_menu_node_tree_get_string_children (menu->priv->tree,
                                                    MARKON_MENU_NODE_TYPE_DIRECTORY_DIR,
                                                    TRUE);

  if (menu->priv->parent != NULL)
    dirs = g_list_concat (dirs, markon_menu_get_directory_dirs (menu->priv->parent));

  return dirs;
}



static MarkonMenuDirectory *
markon_menu_lookup_directory (MarkonMenu  *menu,
                              const gchar *filename)
{
  MarkonMenuDirectory *directory = NULL;
  GList               *dirs = NULL;
  GList               *iter;
  GFile               *file;
  GFile               *dir;
  gboolean             found = FALSE;

  g_return_val_if_fail (MARKON_IS_MENU (menu), NULL);
  g_return_val_if_fail (filename != NULL, NULL);

  dirs = markon_menu_get_directory_dirs (menu);

  /* Iterate through all directories */
  for (iter = dirs; !found && iter != NULL; iter = g_list_next (iter))
    {
      dir = _markon_file_new_relative_to_file (iter->data, menu->priv->file);
      file = _markon_file_new_relative_to_file (filename, dir);

      /* Check if the file exists and is readable */
      if (G_LIKELY (g_file_query_exists (file, NULL)))
        {
          /* Load menu directory */
          directory = markon_menu_directory_new (file);

          /* Update search status */
          found = TRUE;
        }

      /* Destroy the file objects */
      g_object_unref (file);
      g_object_unref (dir);
    }

  /* Free reverse copy */
  g_list_free (dirs);

  return directory;
}



static GList *
markon_menu_get_app_dirs (MarkonMenu *menu,
                          gboolean    recursive)
{
  GList *dirs = NULL;
  GList *lp;
  GList *sp;
  GList *submenu_app_dirs;

  /* Fetch all application directories */
  dirs = markon_menu_node_tree_get_string_children (menu->priv->tree,
                                                    MARKON_MENU_NODE_TYPE_APP_DIR,
                                                    TRUE);

  if (recursive)
    {
      for (lp = menu->priv->submenus; lp != NULL; lp = lp->next)
        {
          submenu_app_dirs = markon_menu_get_app_dirs (lp->data, recursive);

          for (sp = g_list_last (submenu_app_dirs); sp != NULL; sp = sp->prev)
            if (g_list_find_custom (dirs, sp->data, (GCompareFunc) g_strcmp0) == NULL)
              dirs = g_list_prepend (dirs, sp->data);
        }
    }

  return dirs;
}



static void
markon_menu_collect_files (MarkonMenu *menu,
                           GHashTable *desktop_id_table)
{
  GList *app_dirs = NULL;
  GList *iter;
  GFile *file;

  g_return_if_fail (MARKON_IS_MENU (menu));

  app_dirs = markon_menu_get_app_dirs (menu, FALSE);

  /* Collect desktop entry filenames */
  for (iter = app_dirs; iter != NULL; iter = g_list_next (iter))
    {
      file = g_file_new_for_uri (iter->data);
      markon_menu_collect_files_from_path (menu, desktop_id_table, file, NULL);
      g_object_unref (file);
    }

  /* Free directory list */
  g_list_free (app_dirs);

  /* Collect filenames for submenus */
  for (iter = menu->priv->submenus; iter != NULL; iter = g_list_next (iter))
    markon_menu_collect_files (iter->data, desktop_id_table);
}



static void
markon_menu_collect_files_from_path (MarkonMenu  *menu,
                                     GHashTable  *desktop_id_table,
                                     GFile       *dir,
                                     const gchar *id_prefix)
{
  GFileEnumerator *enumerator;
  GFileInfo       *file_info;
  GFile           *file;
  gchar           *base_name;
  gchar           *new_id_prefix;
  gchar           *desktop_id;

  g_return_if_fail (MARKON_IS_MENU (menu));

  /* Skip directory if it doesn't exist */
  if (G_UNLIKELY (!g_file_query_exists (dir, NULL)))
    return;

  /* Skip directory if it's not a directory */
  if (G_UNLIKELY (g_file_query_file_type (dir, G_FILE_QUERY_INFO_NONE,
                                          NULL) != G_FILE_TYPE_DIRECTORY))
    {
      return;
    }

  /* Open directory for reading */
  enumerator = g_file_enumerate_children (dir, "standard::name,standard::type",
                                          G_FILE_QUERY_INFO_NONE, NULL, NULL);

  /* Abort if directory cannot be opened */
  if (G_UNLIKELY (enumerator == NULL))
    return;

  /* Read file by file */
  while (TRUE)
    {
      file_info = g_file_enumerator_next_file (enumerator, NULL, NULL);

      if (G_UNLIKELY (file_info == NULL))
        break;

      file = g_file_resolve_relative_path (dir, g_file_info_get_name (file_info));
      base_name = g_file_get_basename (file);

      /* Treat files and directories differently */
      if (g_file_info_get_file_type (file_info) == G_FILE_TYPE_DIRECTORY)
        {
          /* Create new desktop-file id prefix */
          if (G_LIKELY (id_prefix == NULL))
            new_id_prefix = g_strdup (base_name);
          else
            new_id_prefix = g_strjoin ("-", id_prefix, base_name, NULL);

          /* Collect files in the directory */
          markon_menu_collect_files_from_path (menu, desktop_id_table, file, new_id_prefix);

          /* Free id prefix */
          g_free (new_id_prefix);
        }
      else if (G_LIKELY (g_str_has_suffix (base_name, ".desktop")))
        {
          /* Create desktop-file id */
          if (G_LIKELY (id_prefix == NULL))
            desktop_id = g_strdup (base_name);
          else
            desktop_id = g_strjoin ("-", id_prefix, base_name, NULL);

          /* Insert into the files hash table if the desktop-file id does not exist there yet */
          if (G_LIKELY (g_hash_table_lookup (desktop_id_table, desktop_id) == NULL))
            g_hash_table_insert (desktop_id_table, desktop_id, g_file_get_uri (file));
          else
            g_free (desktop_id);
        }

      /* Free absolute path */
      g_free (base_name);

      /* Destroy file */
      g_object_unref (file);

      /* Destroy info */
      g_object_unref (file_info);
    }

  g_object_unref (enumerator);
}



static gboolean
collect_rules (GNode   *node,
               GSList **list)
{
  MarkonMenuNodeType type;

  type = markon_menu_node_tree_get_node_type (node);

  if (type == MARKON_MENU_NODE_TYPE_INCLUDE ||
      type == MARKON_MENU_NODE_TYPE_EXCLUDE)
    {
      *list = g_slist_append (*list, node);
    }

  return FALSE;
}



static void
markon_menu_resolve_items (MarkonMenu *menu,
                           GHashTable *desktop_id_table,
                           gboolean    only_unallocated)
{
  GSList  *rules = NULL;
  GSList  *iter;
  GList   *submenu;
  gboolean menu_only_unallocated = FALSE;

  g_return_if_fail (MARKON_IS_MENU (menu));

  menu_only_unallocated =
    markon_menu_node_tree_get_boolean_child (menu->priv->tree,
                                             MARKON_MENU_NODE_TYPE_ONLY_UNALLOCATED);

  /* Resolve items in this menu (if it matches the only_unallocated argument.
   * This means that in the first pass, all items of menus without
   * <OnlyUnallocated /> are resolved and in the second pass, only items of
   * menus with <OnlyUnallocated /> are resolved */
  if (menu_only_unallocated == only_unallocated)
    {
      g_node_traverse (menu->priv->tree, G_IN_ORDER, G_TRAVERSE_ALL, 2,
                       (GNodeTraverseFunc) collect_rules, &rules);

      /* Iterate over all rules */
      for (iter = rules; iter != NULL; iter = g_slist_next (iter))
        {
          if (G_LIKELY (markon_menu_node_tree_get_node_type (iter->data) == MARKON_MENU_NODE_TYPE_INCLUDE))
            {
              /* Resolve available items and match them against this rule */
              markon_menu_resolve_items_by_rule (menu, desktop_id_table, iter->data);
            }
          else
            {
              /* Remove all items matching this exclude rule from the item pool */
              markon_menu_item_pool_apply_exclude_rule (menu->priv->pool, iter->data);
            }
        }

      /* Cleanup */
      g_slist_free (rules);
    }

  /* Iterate over all submenus */
  for (submenu = menu->priv->submenus; submenu != NULL; submenu = g_list_next (submenu))
    {
      /* Resolve items of the submenu */
      markon_menu_resolve_items (MARKON_MENU (submenu->data), desktop_id_table,
                                 only_unallocated);
    }
}



static void
markon_menu_resolve_items_by_rule (MarkonMenu *menu,
                                   GHashTable *desktop_id_table,
                                   GNode      *node)
{
  MarkonMenuPair pair;

  g_return_if_fail (MARKON_IS_MENU (menu));

  /* Store menu and rule pointer in the pair */
  pair.first = menu;
  pair.second = node;

  /* Try to insert each of the collected desktop entry filenames into the menu */
  g_hash_table_foreach (desktop_id_table, (GHFunc) markon_menu_resolve_item_by_rule, &pair);
}



static void
markon_menu_resolve_item_by_rule (const gchar    *desktop_id,
                                  const gchar    *uri,
                                  MarkonMenuPair *data)
{
  MarkonMenuItem *item = NULL;
  MarkonMenu     *menu = NULL;
  GNode          *node = NULL;
  gboolean        only_unallocated = FALSE;

  g_return_if_fail (MARKON_IS_MENU (data->first));
  g_return_if_fail (data->second != NULL);

  /* Restore menu and rule from the data pair */
  menu = data->first;
  node = data->second;

  /* Try to load the menu item from the cache */
  item = markon_menu_item_cache_lookup (menu->priv->cache, uri, desktop_id);

  if (G_LIKELY (item != NULL))
    {
      only_unallocated = markon_menu_node_tree_get_boolean_child (menu->priv->tree,
                                                                  MARKON_MENU_NODE_TYPE_ONLY_UNALLOCATED);

      /* Only include item if menu not only includes unallocated items
       * or if the item is not allocated yet */
      if (!only_unallocated || markon_menu_item_get_allocated (item) == 0)
        {
          /* Add item to the pool if it matches the include rule */
          if (G_LIKELY (markon_menu_node_tree_rule_matches (node, item)))
            markon_menu_item_pool_insert (menu->priv->pool, item);
        }
    }
}



static void
markon_menu_remove_deleted_menus (MarkonMenu *menu)
{
  MarkonMenu *submenu;
  GList      *iter;
  gboolean    deleted;

  g_return_if_fail (MARKON_IS_MENU (menu));

  /* Note: There's a limitation: if the root menu has a <Deleted/> we
   * can't just free the pointer here. Therefor we only check child menus. */

  for (iter = menu->priv->submenus; iter != NULL; iter = g_list_next (iter))
    {
      submenu = iter->data;

      /* Check whether there is a <Deleted/> element */
      deleted = markon_menu_node_tree_get_boolean_child (submenu->priv->tree,
                                                         MARKON_MENU_NODE_TYPE_DELETED);

      /* Determine whether this submenu was deleted */
      if (G_LIKELY (submenu->priv->directory != NULL))
        deleted = deleted || markon_menu_directory_get_hidden (submenu->priv->directory);

      /* Remove submenu if it is deleted, otherwise check submenus of the submenu */
      if (G_UNLIKELY (deleted))
        {
          /* Remove submenu from the list ... */
          menu->priv->submenus = g_list_remove_link (menu->priv->submenus, iter);

          /* ... and destroy it */
          g_object_unref (submenu);
        }
      else
        markon_menu_remove_deleted_menus (submenu);
    }
}


/**
 * markon_menu_get_item_pool:
 * @menu: a #MarkonMenu.
 *
 * Get the item pool of the menu. This pool contains all items in this
 * menu (for that of its submenus).
 *
 * Returns: (transfer full): a #MarkonMenuItemPool.
 */
MarkonMenuItemPool*
markon_menu_get_item_pool (MarkonMenu *menu)
{
  g_return_val_if_fail (MARKON_IS_MENU (menu), NULL);

  return menu->priv->pool;
}



static void
items_collect (const gchar    *desktop_id,
               MarkonMenuItem *item,
               GList         **listp)
{
  *listp = g_list_prepend (*listp, item);
}



/**
 * markon_menu_get_items:
 * @menu: a #MarkonMenu.
 *
 * Returns all #MarkonMenuItem included in @menu. The items are
 * sorted by their display names in ascending order.
 *
 * The caller is responsible to free the returned list using
 * g_list_free() when no longer needed.
 *
 * Returns: (element-type MarkonMenuItem) (transfer full): list
 * of #MarkonMenuItem included in @menu.
 */
GList *
markon_menu_get_items (MarkonMenu *menu)
{
  GList *items = NULL;

  g_return_val_if_fail (MARKON_IS_MENU (menu), NULL);

  /* Collect the items in the pool */
  markon_menu_item_pool_foreach (menu->priv->pool, (GHFunc) items_collect, &items);

  /* Sort items */
  items = g_list_sort (items, (GCompareFunc) markon_menu_compare_items);

  return items;
}



static GNode *
markon_menu_get_layout (MarkonMenu *menu,
                        gboolean    default_only)
{
  GNode *layout = NULL;

  g_return_val_if_fail (MARKON_IS_MENU (menu), NULL);

  if (G_LIKELY (!default_only))
    {
      layout = markon_menu_node_tree_get_child_node (menu->priv->tree,
                                                     MARKON_MENU_NODE_TYPE_LAYOUT,
                                                     TRUE);
    }

  if (layout == NULL)
    {
      layout = markon_menu_node_tree_get_child_node (menu->priv->tree,
                                                     MARKON_MENU_NODE_TYPE_DEFAULT_LAYOUT,
                                                     TRUE);

      if (layout == NULL && menu->priv->parent != NULL)
        layout = markon_menu_get_layout (menu->priv->parent, TRUE);
    }

  return layout;
}



static gboolean
layout_has_menuname (GNode       *layout,
                     const gchar *name)
{
  GList   *nodes;
  GList   *iter;
  gboolean has_menuname = FALSE;

  nodes = markon_menu_node_tree_get_child_nodes (layout, MARKON_MENU_NODE_TYPE_MENUNAME,
                                                 FALSE);

  for (iter = g_list_first (nodes); !has_menuname && iter != NULL; iter = g_list_next (iter))
    if (g_str_equal (markon_menu_node_tree_get_string (iter->data), name))
      has_menuname = TRUE;

  g_list_free (nodes);

  return has_menuname;
}



static gboolean
layout_has_filename (GNode       *layout,
                     const gchar *desktop_id)
{
  GList   *nodes;
  GList   *iter;
  gboolean has_filename = FALSE;

  nodes = markon_menu_node_tree_get_child_nodes (layout, MARKON_MENU_NODE_TYPE_FILENAME,
                                                 FALSE);

  for (iter = g_list_first (nodes); !has_filename && iter != NULL; iter = g_list_next (iter))
    if (g_str_equal (markon_menu_node_tree_get_string (iter->data), desktop_id))
      has_filename = TRUE;

  g_list_free (nodes);

  return has_filename;
}



static void
layout_elements_collect (GList **dest_list,
                         GList  *src_list,
                         GNode  *layout)
{
  GList *iter;

  for (iter = src_list; iter != NULL; iter = g_list_next (iter))
    {
      if (MARKON_IS_MENU (iter->data))
        {
          if (G_LIKELY (!layout_has_menuname (layout, markon_menu_get_name (iter->data))))
            *dest_list = g_list_prepend (*dest_list, iter->data);
        }
      else if (MARKON_IS_MENU_ITEM (iter->data))
        {
          if (G_LIKELY (!layout_has_filename (layout, markon_menu_item_get_desktop_id (iter->data))))
            *dest_list = g_list_prepend (*dest_list, iter->data);
        }
    }
}



/**
 * markon_menu_get_elements:
 * @menu: a #MarkonMenu.
 *
 * Get all the menu element in @menu. This contains sub menus, menu items
 * and separators.
 *
 * Returns a list of #MarkonMenuItem or %NULL. Free the list with
 * g_list_free().
 *
 * Returns: (element-type MarkonMenuItem) (nullable) (transfer full):
 */
GList *
markon_menu_get_elements (MarkonMenu *menu)
{
  MarkonMenuLayoutMergeType merge_type;
  MarkonMenuNodeType        type;
  MarkonMenuItem           *item;
  MarkonMenu               *submenu;
  GList                    *items = NULL;
  GList                    *menu_items;
  GNode                    *layout = NULL;
  GNode                    *node;

  g_return_val_if_fail (MARKON_IS_MENU (menu), NULL);

  /* Determine layout node */
  layout = markon_menu_get_layout (menu, FALSE);

  /* There should always be a layout, otherwise MarkonMenuMerger is broken */
  g_return_val_if_fail (layout != NULL, NULL);

  /* Process layout nodes in order */
  for (node = g_node_first_child (layout); node != NULL; node = g_node_next_sibling (node))
    {
      /* Determine layout node type */
      type = markon_menu_node_tree_get_node_type (node);

      if (type == MARKON_MENU_NODE_TYPE_FILENAME)
        {
          /* Search for desktop ID in the item pool */
          item = markon_menu_item_pool_lookup (menu->priv->pool,
                                               markon_menu_node_tree_get_string (node));

          /* If the item with this desktop ID is included in the menu, append it to the list */
          if (G_LIKELY (item != NULL))
            items = g_list_prepend (items, item);
        }
      else if (type == MARKON_MENU_NODE_TYPE_MENUNAME)
        {
          /* Search submenu with this name */
          submenu = markon_menu_get_menu_with_name (menu,
                                                    markon_menu_node_tree_get_string (node));

          /* If there is such a menu, append it to the list */
          if (G_LIKELY (submenu != NULL))
            items = g_list_prepend (items, submenu);
        }
      else if (type == MARKON_MENU_NODE_TYPE_SEPARATOR)
        {
          /* Append separator to the list */
          items = g_list_prepend (items, markon_menu_separator_get_default ());
        }
      else if (type == MARKON_MENU_NODE_TYPE_MERGE)
        {
          /* Determine merge type */
          merge_type = markon_menu_node_tree_get_layout_merge_type (node);

          if (merge_type == MARKON_MENU_LAYOUT_MERGE_ALL)
            {
              /* Get all the submenus */
              menu_items = g_list_copy (menu->priv->submenus);

              /* Get all menu items of this menu */
              markon_menu_item_pool_foreach (menu->priv->pool, (GHFunc) items_collect, &menu_items);

              /* Sort menu items */
              menu_items = g_list_sort (menu_items, (GCompareFunc) markon_menu_compare_items);

              /* Prepend menu items to the returned item list */
              layout_elements_collect (&items, menu_items, layout);

              /* Cleanup */
              g_list_free (menu_items);
            }
          else if (merge_type == MARKON_MENU_LAYOUT_MERGE_FILES)
            {
              /* Get all menu items of this menu */
              menu_items = markon_menu_get_items (menu);

              /* Prepend menu items to the returned item list */
              layout_elements_collect (&items, menu_items, layout);

              /* Cleanup */
              g_list_free (menu_items);
            }
          else if (merge_type == MARKON_MENU_LAYOUT_MERGE_MENUS)
            {
              /* Get all submenus */
              menu_items = markon_menu_get_menus (menu);

              /* Prepend submenus to the returned item list */
              layout_elements_collect (&items, menu_items, layout);

              /* Cleanup */
              g_list_free (menu_items);
            }
        }
    }

  return g_list_reverse (items);
}



static gint
markon_menu_compare_items (gconstpointer *a,
                           gconstpointer *b)
{
  gchar *casefold_a, *casefold_b;
  gint   result;

  /* do case insensitive sorting, see bug #10594 */
  /* TODO: this function is called often, maybe catch the casefolded name */
  casefold_a = g_utf8_casefold (markon_menu_element_get_name (MARKON_MENU_ELEMENT (a)), -1);
  casefold_b = g_utf8_casefold (markon_menu_element_get_name (MARKON_MENU_ELEMENT (b)), -1);

  result = g_utf8_collate (casefold_a, casefold_b);

  g_free (casefold_a);
  g_free (casefold_b);

  return result;
}



static const gchar*
markon_menu_get_element_name (MarkonMenuElement *element)
{
  MarkonMenu    *menu;
  const gchar   *name = NULL;

  g_return_val_if_fail (MARKON_IS_MENU (element), NULL);

  menu = MARKON_MENU (element);

  /* Try directory name first */
  if (menu->priv->directory != NULL)
    name = markon_menu_directory_get_name (menu->priv->directory);

  /* Otherwise use the menu name as a fallback */
  if (name == NULL)
    name = markon_menu_get_name (menu);

  return name;
}



static const gchar*
markon_menu_get_element_comment (MarkonMenuElement *element)
{
  MarkonMenu *menu;

  g_return_val_if_fail (MARKON_IS_MENU (element), NULL);

  menu = MARKON_MENU (element);

  if (menu->priv->directory == NULL)
    return NULL;
  else
    return markon_menu_directory_get_comment (menu->priv->directory);
}



static const gchar*
markon_menu_get_element_icon_name (MarkonMenuElement *element)
{
  MarkonMenu *menu;

  g_return_val_if_fail (MARKON_IS_MENU (element), NULL);

  menu = MARKON_MENU (element);

  if (menu->priv->directory == NULL)
    return NULL;
  else
    return markon_menu_directory_get_icon_name (menu->priv->directory);
}



static gboolean
markon_menu_get_element_visible (MarkonMenuElement *element)
{
  MarkonMenu *menu;
  GList      *items;
  GList      *iter;
  gboolean    visible = FALSE;

  g_return_val_if_fail (MARKON_IS_MENU (element), FALSE);

  menu = MARKON_MENU (element);

  if (menu->priv->directory != NULL)
    {
      if (!markon_menu_directory_get_visible (menu->priv->directory))
        return FALSE;
    }

  /* if a menu has no visible children it shouldn't be visible */
  items = markon_menu_get_elements (menu);
  for (iter = items; visible != TRUE && iter != NULL; iter = g_list_next (iter))
    {
      if (markon_menu_element_get_visible (MARKON_MENU_ELEMENT (iter->data)))
        visible = TRUE;
    }

  g_list_free (items);
  return visible;
}



static gboolean
markon_menu_get_element_show_in_environment (MarkonMenuElement *element)
{
  MarkonMenu *menu;

  g_return_val_if_fail (MARKON_IS_MENU (element), FALSE);

  menu = MARKON_MENU (element);

  if (menu->priv->directory == NULL)
    return FALSE;
  else
    return markon_menu_directory_get_show_in_environment (menu->priv->directory);
}



static gboolean
markon_menu_get_element_no_display (MarkonMenuElement *element)
{
  MarkonMenu *menu;

  g_return_val_if_fail (MARKON_IS_MENU (element), FALSE);

  menu = MARKON_MENU (element);

  if (menu->priv->directory == NULL)
    return FALSE;
  else
    return markon_menu_directory_get_no_display (menu->priv->directory);
}



static gboolean
markon_menu_get_element_equal (MarkonMenuElement *element,
                               MarkonMenuElement *other)
{
  g_return_val_if_fail (MARKON_IS_MENU (element), FALSE);
  g_return_val_if_fail (MARKON_IS_MENU (other), FALSE);

  return MARKON_MENU (element) == MARKON_MENU (other);
}



static void
markon_menu_start_monitoring (MarkonMenu *menu)
{
  GList *lp;

  g_return_if_fail (MARKON_IS_MENU (menu));

  /* Let only the root menu monitor menu files, merge files/directories and app dirs */
  if (menu->priv->parent == NULL)
    {
      /* Create the list for merging consecutive file change events */
      menu->priv->changed_files = NULL;

      /* Reset the idle source for handling file changes */
      menu->priv->file_changed_idle = 0;

      markon_menu_monitor_menu_files (menu);

      markon_menu_monitor_files (menu, menu->priv->merge_files,
                                 markon_menu_merge_file_changed);

      markon_menu_monitor_files (menu, menu->priv->merge_dirs,
                                 markon_menu_merge_dir_changed);

      markon_menu_monitor_app_dirs (menu);
    }

  markon_menu_monitor_directory_dirs (menu);

  /* Recurse into submenus */
  for (lp = menu->priv->submenus; lp != NULL; lp = lp->next)
    markon_menu_start_monitoring (lp->data);

}



static void
markon_menu_stop_monitoring (MarkonMenu *menu)
{
  GList *lp;

  g_return_if_fail (MARKON_IS_MENU (menu));

  /* Recurse into submenus */
  for (lp = menu->priv->submenus; lp != NULL; lp = lp->next)
    markon_menu_stop_monitoring (lp->data);

  /* Disconnect and destroy all monitors */
  for (lp = menu->priv->monitors; lp != NULL; lp = lp->next)
    {
      g_signal_handlers_disconnect_matched (lp->data, G_SIGNAL_MATCH_DATA,
                                            0, 0, NULL, NULL, menu);
      g_object_unref (lp->data);
    }

  /* Free the monitor list */
  g_list_free (menu->priv->monitors);
  menu->priv->monitors = NULL;

  /* Stop the idle source for handling file changes from being invoked */
  if (menu->priv->file_changed_idle != 0)
    g_source_remove (menu->priv->file_changed_idle);

  /* Free the hash table for merging consecutive file change events */
  _markon_g_slist_free_full (menu->priv->changed_files, g_object_unref);
  menu->priv->changed_files = NULL;
}



static void
markon_menu_monitor_menu_files (MarkonMenu *menu)
{
  GFileMonitor *monitor;
  const gchar  *prefix;
  GFile        *file;
  gchar        *relative_filename;
  gchar       **paths;
  gint          n;

  g_return_if_fail (MARKON_IS_MENU (menu));

  if (menu->priv->uses_custom_path)
    {
      /* Monitor the root .menu file */
      monitor = g_file_monitor (menu->priv->file, G_FILE_MONITOR_NONE, NULL, NULL);
      if (monitor != NULL)
        {
          menu->priv->monitors = g_list_prepend (menu->priv->monitors, monitor);
          g_signal_connect_swapped (monitor, "changed",
                                    G_CALLBACK (markon_menu_file_changed), menu);
        }
    }
  else
    {
      /* Build ${XDG_MENU_PREFIX}applications.menu filename */
      prefix = g_getenv ("XDG_MENU_PREFIX");
      relative_filename = g_strconcat ("menus", G_DIR_SEPARATOR_S,
                                       prefix != NULL ? prefix : MARKON_DEFAULT_MENU_PREFIX,
                                       "applications.menu", NULL);

      /* Monitor all application menu candidates */
      paths = markon_config_build_paths (relative_filename);

      for (n = g_strv_length (paths)-1; paths != NULL && n >= 0; --n)
        {
          file = g_file_new_for_path (paths[n]);

          monitor = g_file_monitor (file, G_FILE_MONITOR_NONE, NULL, NULL);
          if (monitor != NULL)
            {
              menu->priv->monitors = g_list_prepend (menu->priv->monitors, monitor);
              g_signal_connect_swapped (monitor, "changed",
                                        G_CALLBACK (markon_menu_file_changed), menu);
            }

          g_object_unref (file);
        }

      /* clean up strings */
      g_strfreev (paths);
      g_free (relative_filename);
    }
}



static gint
find_file_monitor (GFileMonitor *monitor,
                   GFile        *file)
{
  GFile *monitored_file;

  monitored_file = g_object_get_qdata (G_OBJECT (monitor), markon_menu_file_quark);

  if (monitored_file != NULL && g_file_equal (monitored_file, file))
    return 0;
  else
    return -1;
}



static void
markon_menu_monitor_files (MarkonMenu *menu,
                           GList      *files,
                           gpointer    callback)
{
  GFileMonitor *monitor;
  GList        *lp;

  g_return_if_fail (MARKON_IS_MENU (menu));
  g_return_if_fail (menu->priv->parent == NULL);

  /* Monitor all files from the list */
  for (lp = files; lp != NULL; lp = lp->next)
    {
      /* Monitor files only if they are not being monitored already */
      if (g_list_find_custom (menu->priv->monitors, lp->data,
                              (GCompareFunc) find_file_monitor) == NULL)
        {
          /* Try to monitor the file */
          monitor = g_file_monitor (lp->data, G_FILE_MONITOR_NONE, NULL, NULL);
          if (monitor != NULL)
            {
              /* Associate the monitor with the monitored file */
              g_object_set_qdata_full (G_OBJECT (monitor), markon_menu_file_quark,
                                       g_object_ref (lp->data), g_object_unref);

              /* Add the monitor to the list of monitors belonging to the menu */
              menu->priv->monitors = g_list_prepend (menu->priv->monitors, monitor);

              /* Make sure we are notified when the file changes */
              g_signal_connect_swapped (monitor, "changed", G_CALLBACK (callback), menu);
            }
        }
    }
}



static void
markon_menu_monitor_app_dirs (MarkonMenu *menu)
{
  GFile *dir;
  GList *app_dirs;
  GList *dirs = NULL;
  GList *lp;

  g_return_if_fail (MARKON_IS_MENU (menu));
  g_return_if_fail (menu->priv->parent == NULL);

  g_return_if_fail (MARKON_IS_MENU (menu));

  /* Determine all application directories we are interested in for this menu */
  app_dirs = markon_menu_get_app_dirs (menu, TRUE);

  /* Transform app dir filenames into GFile objects, resolving filenames
   * relative to the menu file itself */
  for (lp = app_dirs; lp != NULL; lp = lp->next)
    {
      dir = _markon_file_new_relative_to_file (lp->data, menu->priv->file);
      dirs = g_list_prepend (dirs, dir);
    }

  /* Monitor the app dirs */
  markon_menu_monitor_files (menu, dirs, markon_menu_app_dir_changed);

  /* Release the allocated GFiles and free the list */
  _markon_g_list_free_full (dirs, g_object_unref);

  /* Free app dir list */
  g_list_free (app_dirs);
}



static void
markon_menu_monitor_directory_dirs (MarkonMenu *menu)
{
  GFileMonitor *monitor;
  GFile        *file;
  GFile        *dir;
  GList        *directory_files;
  GList        *directory_dirs;
  GList        *dp;
  GList        *lp;

  g_return_if_fail (MARKON_IS_MENU (menu));

  /* Determine all .directory files we are interested in for this menu */
  directory_files = markon_menu_get_directories (menu);

  /* Determine all .directory lookup dirs for this menu */
  directory_dirs = markon_menu_get_directory_dirs (menu);

  /* Monitor potential .directory files */
  for (lp = directory_files; lp != NULL; lp = lp->next)
    {
      for (dp = directory_dirs; dp != NULL; dp = dp->next)
        {
          dir = _markon_file_new_relative_to_file (dp->data, menu->priv->file);
          file = _markon_file_new_relative_to_file (lp->data, dir);

          /* Only try to monitor the .directory file if we don't do that already */
          if (g_list_find_custom (menu->priv->monitors, file,
                                  (GCompareFunc) find_file_monitor) == NULL)
            {
              /* Try to monitor the file */
              monitor = g_file_monitor (file, G_FILE_MONITOR_NONE, NULL, NULL);
              if (monitor != NULL)
                {
                  /* Associate the monitor with the monitored file */
                  g_object_set_qdata_full (G_OBJECT (monitor), markon_menu_file_quark,
                                           g_object_ref (file), g_object_unref);

                  /* Add the monitor to the list of monitors belonging to the menu */
                  menu->priv->monitors = g_list_prepend (menu->priv->monitors, monitor);

                  /* Make sure we are notified when the file changes */
                  g_signal_connect_swapped (monitor, "changed",
                                            G_CALLBACK (markon_menu_directory_file_changed),
                                            menu);
                }
            }

          g_object_unref (file);
          g_object_unref (dir);
        }
    }

  /* Free lists */
  g_list_free (directory_dirs);
  g_list_free (directory_files);
}



#ifdef DEBUG
static void
markon_menu_debug (GFile             *file,
                   GFileMonitorEvent  event_type,
                   const gchar       *comment)
{
  gchar *path;
  gchar *msg = NULL;

  if (file != NULL)
    {
      path = g_file_get_path (file);
      msg = g_strdup_printf ("%s (%d, %s)", comment, event_type, path);
      g_free (path);

      comment = msg;
    }

  g_print ("%s\n", comment);
  g_free (msg);
}
#else
#define markon_menu_debug(...) G_STMT_START{ (void)0; }G_STMT_END
#endif




static gboolean
markon_menu_file_emit_reload_required_idle (gpointer data)
{
  MarkonMenu *menu = MARKON_MENU (data);

  g_return_val_if_fail (MARKON_IS_MENU (menu), FALSE);

  menu->priv->idle_reload_required_id = 0;

  markon_menu_debug (NULL, 0, "emit reload-required");

  g_signal_emit (menu, menu_signals[RELOAD_REQUIRED], 0);

  return FALSE;
}



static void
markon_menu_file_emit_reload_required (MarkonMenu *menu)
{
  if (menu->priv->idle_reload_required_id == 0)
    {
      markon_menu_debug (NULL, 0, "schedule idle menu reload");
      menu->priv->idle_reload_required_id =
        g_idle_add (markon_menu_file_emit_reload_required_idle, menu);
    }
}



static void
markon_menu_file_changed (MarkonMenu       *menu,
                          GFile            *file,
                          GFile            *other_file,
                          GFileMonitorEvent event_type,
                          GFileMonitor     *monitor)
{
  const gchar *prefix;
  gboolean     higher_priority = FALSE;
  gboolean     lower_priority = FALSE;
  GFile       *menu_file;
  gchar      **paths;
  gchar       *relative_filename;
  guint        n;

  g_return_if_fail (MARKON_IS_MENU (menu));
  g_return_if_fail (menu->priv->parent == NULL);

  /* Quick check: reloading is needed if the menu file being used has changed */
  if (g_file_equal (menu->priv->file, file))
    {
      markon_menu_debug (file, event_type, "menu changed");
      markon_menu_file_emit_reload_required (menu);
      return;
    }

  /* If we receive a delete event here, then abort, because there is no
   * need to refresh if a menu file is removed that we're not using because
   * it is lower in priority (else we'd be using it already) */
  if (event_type == G_FILE_MONITOR_EVENT_DELETED)
    return;

  /* Build the ${XDG_MENU_PREFIX}applications.menu filename */
  prefix = g_getenv ("XDG_MENU_PREFIX");
  relative_filename = g_strconcat ("menus", G_DIR_SEPARATOR_S,
                                   prefix != NULL ? prefix : MARKON_DEFAULT_MENU_PREFIX,
                                   "applications.menu", NULL);

  /* Get XDG config paths for the root spec (e.g. menus/expidus-applications.menu) */
  paths = markon_config_build_paths (relative_filename);

  /* Check if the event file has higher priority than the file currently being used */
  for (n = 0;
       !lower_priority && !higher_priority && paths != NULL && paths[n] != NULL;
       ++n)
    {
      menu_file = g_file_new_for_path (paths[n]);

      if (g_file_equal (menu_file, menu->priv->file))
        {
          /* the menu's file comes before the changed file in the load
           * priority order, so the changed file has a lower priority */
          lower_priority = TRUE;
        }
      else if (g_file_equal (menu_file, file))
        {
          /* the changed file comes before the menu's file in the load
           * priority order, so the changed file has a higher priority */
          higher_priority = TRUE;
        }

      g_object_unref (menu_file);
    }

  /* clean up */
  g_free (relative_filename);

  /* If the event file has higher priority, a menu reload is needed */
  if (!lower_priority
      && higher_priority)
    {
      markon_menu_debug (file, event_type, "new menu has higher prio");
      markon_menu_file_emit_reload_required (menu);
    }
}



static void
markon_menu_merge_file_changed (MarkonMenu       *menu,
                                GFile            *file,
                                GFile            *other_file,
                                GFileMonitorEvent event_type,
                                GFileMonitor     *monitor)
{
  g_return_if_fail (MARKON_IS_MENU (menu));
  g_return_if_fail (menu->priv->parent == NULL);

  markon_menu_debug (file, event_type, "merge file changed");
  markon_menu_file_emit_reload_required (menu);
}



static void
markon_menu_merge_dir_changed (MarkonMenu       *menu,
                               GFile            *file,
                               GFile            *other_file,
                               GFileMonitorEvent event_type,
                               GFileMonitor     *monitor)
{
  g_return_if_fail (MARKON_IS_MENU (menu));
  g_return_if_fail (menu->priv->parent == NULL);

  markon_menu_debug (file, event_type, "merge dir changed");
  markon_menu_file_emit_reload_required (menu);
}



static gboolean
markon_menu_process_file_changes (gpointer user_data)
{
  MarkonMenu *menu = user_data;
  MarkonMenuItem *item;
  GFileType       file_type;
  gboolean        affects_the_outside = FALSE;
  gboolean        stop_processing = FALSE;
  GFile          *file;
  GSList         *lp;
  gchar          *path;

  g_return_val_if_fail (MARKON_IS_MENU (menu), FALSE);
  g_return_val_if_fail (menu->priv->parent == NULL, FALSE);

  for (lp = menu->priv->changed_files; !stop_processing && lp != NULL; lp = lp->next)
    {
      file = G_FILE (lp->data);

      /* query the type of the changed file */
      file_type = g_file_query_file_type (file, G_FILE_QUERY_INFO_NONE, NULL);

      if (file_type == G_FILE_TYPE_DIRECTORY)
        {
          /* in this situation, an app dir could have
           * - become unreadable for the current user
           * - been deleted
           * - created (possibly inside an existing one)
           * this is not trivial to handle, so we simply enforce a
           * menu reload to deal with the changes */
          markon_menu_debug (file, 0, "proccess directory change");
          markon_menu_file_emit_reload_required (menu);
          stop_processing = TRUE;
        }
      else
        {
          path = g_file_get_path (file);
          if (path != NULL && g_str_has_suffix (path, ".desktop"))
            {
              /* a regular file changed, try to find the corresponding menu item */
              item = markon_menu_find_file_item (menu, file);
              if (item != NULL)
                {
                  /* try to reload the item */
                  if (markon_menu_item_reload (item, &affects_the_outside, NULL))
                    {
                      if (affects_the_outside)
                        {
                          /* if the categories changed, the item might have to be
                           * moved around between different menus. this is slightly
                           * more complicated than one would first think, so just
                           * enforce a complete menu reload for now */
                          markon_menu_file_emit_reload_required (menu);
                          stop_processing = TRUE;
                        }
                      else
                        {
                          /* remove the item from the desktop item cache so we are forced
                           * to reload it from disk the next time */
                          markon_menu_item_cache_invalidate_file (menu->priv->cache, file);

                          /* nothing else to do here. the item should emit a 'changed'
                           * signal to which users of this library can react */
                        }
                    }
                  else
                    {
                      /* remove the item from the desktop item cache so we are forced
                       * to reload it from disk the next time */
                      markon_menu_item_cache_invalidate_file (menu->priv->cache, file);

                      /* failed to reload the menu item. this can have many reasons,
                       * one of them being that the file permissions might have changed
                       * or that the file was deleted. handling most situations can be very
                       * tricky, so, again, we just enfore a menu reload until we have
                       * something better */
                      markon_menu_debug (file, 0, "auto reload failed");
                      markon_menu_file_emit_reload_required (menu);
                      stop_processing = TRUE;
                    }
                }
              else
                {
                  /* there could be a lot of stuff happening here. seriously, this
                   * stuff is complicated. for now, simply enforce a complete reload
                   * of the menu structure */
                  markon_menu_debug (file, 0, "unknown file, full reload");
                  markon_menu_file_emit_reload_required (menu);
                  stop_processing = TRUE;
                }
            }
          g_free (path);
        }
    }

  /* reset the changed files list, all events processed */
  _markon_g_slist_free_full (menu->priv->changed_files, g_object_unref);
  menu->priv->changed_files = NULL;

  /* reset the idle source ID */
  menu->priv->file_changed_idle = 0;

  /* remove the idle source */
  return FALSE;
}



static gint
compare_files (gconstpointer a,
               gconstpointer b)
{
  return g_file_equal (G_FILE (a), G_FILE (b)) ? 0 : 1;
}



static void
markon_menu_app_dir_changed (MarkonMenu       *menu,
                             GFile            *file,
                             GFile            *other_file,
                             GFileMonitorEvent event_type,
                             GFileMonitor     *monitor)
{
  MarkonMenuItem *item;
  GFileType       file_type;

  g_return_if_fail (MARKON_IS_MENU (menu));
  g_return_if_fail (menu->priv->parent == NULL);

  if (event_type == G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT
      || event_type == G_FILE_MONITOR_EVENT_CREATED
      || event_type == G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED)
    {
      /* add the file to the changed files queue if we have no change event for
       * it queued yet */
      if (g_slist_find_custom (menu->priv->changed_files, file, compare_files) == NULL)
        {
          markon_menu_debug (file, event_type, "add file to changed-queue");
          menu->priv->changed_files = g_slist_prepend (menu->priv->changed_files,
                                                       g_object_ref (file));

          /* register the idle handler if it is not active yet */
          if (menu->priv->file_changed_idle == 0)
            {
              markon_menu_debug (NULL, 0, "schedule process file changes");
              menu->priv->file_changed_idle =
                g_idle_add (markon_menu_process_file_changes, menu);
            }
        }
    }
  else if (event_type == G_FILE_MONITOR_EVENT_DELETED)
    {
      /* query the type of the changed file */
      file_type = g_file_query_file_type (file, G_FILE_QUERY_INFO_NONE, NULL);

      if (file_type == G_FILE_TYPE_DIRECTORY)
        {
          /* an existing app dir (or a subdirectory) has been deleted. we
           * could remove all the items that are in use and reside inside
           * this root directory. but for now... enforce a menu reload! */
          markon_menu_debug (file, event_type, "app dir deleted");
          markon_menu_file_emit_reload_required (menu);
        }
      else
        {
          /* a regular file was deleted, try to find the corresponding menu item */
          item = markon_menu_find_file_item (menu, file);
          if (item != NULL)
            {
              /* remove the item from the desktop item cache so we are forced
               * to reload it from disk the next time */
              markon_menu_item_cache_invalidate_file (menu->priv->cache, file);

              /* ok, so a .desktop file was removed. of course we don't know
               * yet whether there is a replacement in another app dir
               * with lower priority. we could try to find out but for now
               * it's easier to simply enforce a menu reload */
              markon_menu_debug (file, event_type, "file deleted");
              markon_menu_file_emit_reload_required (menu);
            }
          else
            {
              /* the deleted file hasn't been in use anyway, so removing it
               * doesn't change anything. so we have nothing to do for a
               * change, no f****ing menu reload! */
            }
        }
    }
}



static void
markon_menu_directory_file_changed (MarkonMenu       *menu,
                                    GFile            *file,
                                    GFile            *other_file,
                                    GFileMonitorEvent event_type,
                                    GFileMonitor     *monitor)
{
  MarkonMenuDirectory *old_directory = NULL;

  g_return_if_fail (MARKON_IS_MENU (menu));

  if (event_type == G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT
      || event_type == G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED
      || event_type == G_FILE_MONITOR_EVENT_DELETED
      || event_type == G_FILE_MONITOR_EVENT_CREATED)
    {
      /* take a reference on the current menu directory */
      if (menu->priv->directory != NULL)
        old_directory = g_object_ref (menu->priv->directory);

      /* reset the menu directory of the menu and load a new one */
      markon_menu_resolve_directory (menu, NULL, FALSE);

      /* Only emit the event if something changed (see bug #8671) */
      if (event_type != G_FILE_MONITOR_EVENT_DELETED
          || (old_directory == NULL) != (menu->priv->directory == NULL)
          || !markon_menu_directory_equal (old_directory, menu->priv->directory))
        {
          markon_menu_debug (file, event_type, "directory changed");

          /* Notify listeners about the old and new menu directories */
          g_signal_emit (menu, menu_signals[DIRECTORY_CHANGED], 0,
                         old_directory, menu->priv->directory);
        }

      /* release the old menu directory we no longer need */
      if (old_directory != NULL)
        g_object_unref (old_directory);
    }
}



static MarkonMenuItem *
markon_menu_find_file_item (MarkonMenu *menu,
                            GFile      *file)
{
  MarkonMenuItem *item = NULL;
  GList          *lp;

  g_return_val_if_fail (MARKON_IS_MENU (menu), NULL);
  g_return_val_if_fail (G_IS_FILE (file), NULL);

  item = markon_menu_item_pool_lookup_file (menu->priv->pool, file);

  if (item == NULL)
    {
      for (lp = menu->priv->submenus; item == NULL && lp != NULL; lp = lp->next)
        item = markon_menu_find_file_item (lp->data, file);
    }

  return item;

}
