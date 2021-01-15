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

#include <locale.h>
#include <glib.h>
#include <libexpidus1util/libexpidus1util.h>

#include <markon/markon-environment.h>
#include <markon/markon-menu-directory.h>
#include <markon/markon-private.h>



/**
 * SECTION: markon-menu-directory
 * @title: MarkonMenuDirectory
 * @short_description: Markon element for .directory files.
 * @include: markon/markon.h
 *
 * Element that represents a .directory file in the menu configurations.
 * Each menu (except for the root menu) has a #MarkonMenuDirectory,
 * see markon_menu_get_directory().
 **/



/* Desktop entry keys */
#if 0
static const gchar *desktop_entry_keys[] =
{
  "Name",
  "Comment",
  "Icon",
  "Categories",
  "OnlyShowIn",
  "NotShowIn",
  "NoDisplay",
  "Hidden",
  NULL
};
#endif



/* Property identifiers */
enum
{
  PROP_0,
  PROP_FILE,
  PROP_NAME,
  PROP_COMMENT,
  PROP_NO_DISPLAY,
  PROP_ICON_NAME,
};



static void markon_menu_directory_finalize     (GObject                  *object);
static void markon_menu_directory_get_property (GObject                  *object,
                                                guint                     prop_id,
                                                GValue                   *value,
                                                GParamSpec               *pspec);
static void markon_menu_directory_set_property (GObject                  *object,
                                                guint                     prop_id,
                                                const GValue             *value,
                                                GParamSpec               *pspec);



struct _MarkonMenuDirectoryPrivate
{
  /* Directory file */
  GFile  *file;

  /* Directory name */
  gchar  *name;

  /* Directory description (comment) */
  gchar  *comment;

  /* Icon */
  gchar  *icon_name;

  /* Environments in which the menu should be displayed only */
  gchar **only_show_in;

  /* Environments in which the menu should be hidden */
  gchar **not_show_in;

  /* Whether the menu should be ignored completely */
  guint   hidden : 1;

  /* Whether the menu should be hidden */
  guint   no_display : 1;
};



/* TODO, maybe implement the MarkonMenuElement interface */
G_DEFINE_TYPE_WITH_PRIVATE (MarkonMenuDirectory, markon_menu_directory, G_TYPE_OBJECT)



static void
markon_menu_directory_class_init (MarkonMenuDirectoryClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = markon_menu_directory_finalize;
  gobject_class->get_property = markon_menu_directory_get_property;
  gobject_class->set_property = markon_menu_directory_set_property;

  /**
   * MarkonMenuDirectory:filename:
   *
   * The @GFile of an %MarkonMenuDirectory.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_FILE,
                                   g_param_spec_object ("file",
                                                        "File",
                                                        "File",
                                                        G_TYPE_FILE,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS |
                                                        G_PARAM_CONSTRUCT_ONLY));

  /**
   * MarkonMenuDirectory:name:
   *
   * Name of the directory.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_NAME,
                                   g_param_spec_string ("name",
                                                        "Name",
                                                        "Directory name",
                                                        NULL,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

  /**
   * MarkonMenuDirectory:comment:
   *
   * Directory description (comment).
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_COMMENT,
                                   g_param_spec_string ("comment",
                                                        "Description",
                                                        "Directory description",
                                                        NULL,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

  /**
   * MarkonMenuDirectory:icon-name:
   *
   * Icon associated with this directory.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_ICON_NAME,
                                   g_param_spec_string ("icon-name",
                                                        "Icon",
                                                        "Directory icon",
                                                        NULL,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

  /**
   * MarkonMenuDirectory:no-display:
   *
   * Whether this menu item is hidden in menus.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_NO_DISPLAY,
                                   g_param_spec_boolean ("no-display",
                                                         "No Display",
                                                         "Visibility state of the related menu",
                                                         FALSE,
                                                         G_PARAM_READWRITE |
                                                         G_PARAM_STATIC_STRINGS));

}



static void
markon_menu_directory_init (MarkonMenuDirectory *directory)
{
  directory->priv = markon_menu_directory_get_instance_private (directory);
  directory->priv->file = NULL;
  directory->priv->name = NULL;
  directory->priv->icon_name = NULL;
  directory->priv->only_show_in = NULL;
  directory->priv->not_show_in = NULL;
  directory->priv->hidden = FALSE;
  directory->priv->no_display = FALSE;
}



static void
markon_menu_directory_finalize (GObject *object)
{
  MarkonMenuDirectory *directory = MARKON_MENU_DIRECTORY (object);

  /* Free name */
  g_free (directory->priv->name);

  /* Free comment */
  g_free (directory->priv->comment);

  /* Free icon_name */
  g_free (directory->priv->icon_name);

  /* Free environment lists */
  g_strfreev (directory->priv->only_show_in);
  g_strfreev (directory->priv->not_show_in);

  /* Free file */
  if (directory->priv->file != NULL)
    g_object_unref (directory->priv->file);

  (*G_OBJECT_CLASS (markon_menu_directory_parent_class)->finalize) (object);
}



static void
markon_menu_directory_get_property (GObject    *object,
                                    guint       prop_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
  MarkonMenuDirectory *directory = MARKON_MENU_DIRECTORY (object);

  switch (prop_id)
    {
    case PROP_FILE:
      g_value_set_object (value, directory->priv->file);
      break;

    case PROP_NAME:
      g_value_set_string (value, markon_menu_directory_get_name (directory));
      break;

    case PROP_COMMENT:
      g_value_set_string (value, markon_menu_directory_get_comment (directory));
      break;

    case PROP_ICON_NAME:
      g_value_set_string (value, markon_menu_directory_get_icon_name (directory));
      break;

    case PROP_NO_DISPLAY:
      g_value_set_boolean (value, markon_menu_directory_get_no_display (directory));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
markon_menu_directory_set_property (GObject      *object,
                                    guint         prop_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  MarkonMenuDirectory *directory = MARKON_MENU_DIRECTORY (object);

  switch (prop_id)
    {
    case PROP_FILE:
      directory->priv->file = g_value_dup_object (value);
      break;

    case PROP_NAME:
      markon_menu_directory_set_name (directory, g_value_get_string (value));
      break;

    case PROP_COMMENT:
      markon_menu_directory_set_comment (directory, g_value_get_string (value));
      break;

    case PROP_ICON_NAME:
      markon_menu_directory_set_icon_name (directory, g_value_get_string (value));
      break;

    case PROP_NO_DISPLAY:
      markon_menu_directory_set_no_display (directory, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



/**
 * markon_menu_directory_new: (method)
 * @file : a #GFile
 *
 * Create a new #MarkonMenuDirectory for @file. You most likely never
 * use this, but retrieve the info from markon_menu_get_directory().
 *
 * Returns: a #MarkonMenuDirectory.
 **/
MarkonMenuDirectory *
markon_menu_directory_new (GFile *file)
{
  MarkonMenuDirectory *directory = NULL;
  ExpidusRc              *rc;
  const gchar         *name;
  const gchar         *comment;
  const gchar         *icon_name;
  gboolean             no_display;
  gchar               *filename;

  g_return_val_if_fail (G_IS_FILE (file), NULL);
  g_return_val_if_fail (g_file_is_native (file), NULL);

 /* Open the rc file */
  filename = g_file_get_path (file);
  rc = expidus_rc_simple_open (filename, TRUE);
  g_free (filename);
  if (G_UNLIKELY (rc == NULL))
    return NULL;

  expidus_rc_set_group (rc, G_KEY_FILE_DESKTOP_GROUP);

  /* Parse name, exec command and icon name */
  name = expidus_rc_read_entry (rc, G_KEY_FILE_DESKTOP_KEY_NAME, NULL);

  /* If there is no name we must bail out now or segfault later */
  if (G_UNLIKELY (name == NULL))
    return NULL;

  comment = expidus_rc_read_entry (rc, G_KEY_FILE_DESKTOP_KEY_COMMENT, NULL);
  icon_name = expidus_rc_read_entry_untranslated (rc, G_KEY_FILE_DESKTOP_KEY_ICON, NULL);
  no_display = expidus_rc_read_bool_entry (rc, G_KEY_FILE_DESKTOP_KEY_NO_DISPLAY, FALSE);

  /* Allocate a new directory instance */
  directory = g_object_new (MARKON_TYPE_MENU_DIRECTORY,
                            "file", file,
                            "name", name,
                            "comment", comment,
                            "icon-name", icon_name,
                            "no-display", no_display,
                            NULL);

  /* Set rest of the private data directly */
  directory->priv->only_show_in = expidus_rc_read_list_entry (rc, G_KEY_FILE_DESKTOP_KEY_ONLY_SHOW_IN, ";");
  directory->priv->not_show_in = expidus_rc_read_list_entry (rc, G_KEY_FILE_DESKTOP_KEY_NOT_SHOW_IN, ";");
  directory->priv->hidden = expidus_rc_read_bool_entry (rc, G_KEY_FILE_DESKTOP_KEY_HIDDEN, FALSE);

  /* Cleanup */
  expidus_rc_close (rc);

  return directory;
}



/**
 * markon_menu_directory_get_file:
 * @directory: a #MarkonMenuDirectory
 *
 * Get the #GFile for @directory. The returned object should be
 * unreffed with g_object_unref() when no longer needed.
 *
 * Returns: (transfer full): a #GFile
 */
GFile *
markon_menu_directory_get_file (MarkonMenuDirectory *directory)
{
  g_return_val_if_fail (MARKON_IS_MENU_DIRECTORY (directory), NULL);
  return g_object_ref (directory->priv->file);
}



/**
 * markon_menu_directory_get_name:
 * @directory : a #MarkonMenuDirectory
 *
 * Get the name of @directory.
 *
 * Returns: a the name for @directory.
 */
const gchar*
markon_menu_directory_get_name (MarkonMenuDirectory *directory)
{
  g_return_val_if_fail (MARKON_IS_MENU_DIRECTORY (directory), NULL);
  return directory->priv->name;
}



/**
 * markon_menu_directory_set_name:
 * @directory : a #MarkonMenuDirectory
 * @name : the new name for @directory.
 *
 * Set the name of @directory.
 */
void
markon_menu_directory_set_name (MarkonMenuDirectory *directory,
                                const gchar         *name)
{
  g_return_if_fail (MARKON_IS_MENU_DIRECTORY (directory));
  g_return_if_fail (g_utf8_validate (name, -1, NULL));

  if (g_strcmp0 (directory->priv->name, name) == 0)
    return;

  /* Free old name */
  g_free (directory->priv->name);

  /* Set the new filename */
  directory->priv->name = g_strdup (name);

  /* Notify listeners */
  g_object_notify (G_OBJECT (directory), "name");
}



/**
 * markon_menu_directory_get_comment:
 * @directory : a #MarkonMenuDirectory
 *
 * Get the comment of @directory.
 *
 * Returns: a the description for @directory.
 */
const gchar*
markon_menu_directory_get_comment (MarkonMenuDirectory *directory)
{
  g_return_val_if_fail (MARKON_IS_MENU_DIRECTORY (directory), NULL);
  return directory->priv->comment;
}



/**
 * markon_menu_directory_set_comment:
 * @directory : a #MarkonMenuDirectory
 * @comment : the new description for @directory.
 *
 * Set the comment of @directory.
 */
void
markon_menu_directory_set_comment (MarkonMenuDirectory *directory,
                                   const gchar         *comment)
{
  g_return_if_fail (MARKON_IS_MENU_DIRECTORY (directory));
  g_return_if_fail (comment == NULL || g_utf8_validate (comment, -1, NULL));

  if (g_strcmp0 (directory->priv->comment, comment) == 0)
    return;

  /* Free old name */
  g_free (directory->priv->comment);

  /* Set the new filename */
  directory->priv->comment = g_strdup (comment);

  /* Notify listeners */
  g_object_notify (G_OBJECT (directory), "comment");
}



/**
 * markon_menu_directory_get_icon_name:
 * @directory : a #MarkonMenuDirectory
 *
 * Get the icon name of @directory.
 *
 * Returns: a the icon-name key for @directory.
 */
const gchar*
markon_menu_directory_get_icon_name (MarkonMenuDirectory *directory)
{
  g_return_val_if_fail (MARKON_IS_MENU_DIRECTORY (directory), NULL);
  return directory->priv->icon_name;
}



/**
 * markon_menu_directory_set_icon_name:
 * @directory : a #MarkonMenuDirectory
 * @icon_name      : the new icon name for @directory.
 *
 * Set the icon name of @directory.
 */
void
markon_menu_directory_set_icon_name (MarkonMenuDirectory *directory,
                                     const gchar         *icon_name)
{
  g_return_if_fail (MARKON_IS_MENU_DIRECTORY (directory));

  if (g_strcmp0 (directory->priv->icon_name, icon_name) == 0)
    return;

  /* Free old name */
  g_free (directory->priv->icon_name);

  /* Set the new filename */
  directory->priv->icon_name = g_strdup (icon_name);

  /* Notify listeners */
  g_object_notify (G_OBJECT (directory), "icon-name");
}



/**
 * markon_menu_directory_get_no_display:
 * @directory : a #MarkonMenuDirectory
 *
 * Whether @directory should be displayed.
 * For applications you want to call markon_menu_directory_get_visible().
 *
 * Returns: a the no-display key for @directory.
 */
gboolean
markon_menu_directory_get_no_display (MarkonMenuDirectory *directory)
{
  g_return_val_if_fail (MARKON_IS_MENU_DIRECTORY (directory), FALSE);
  return directory->priv->no_display;
}



/**
 * markon_menu_directory_set_no_display:
 * @directory : a #MarkonMenuDirectory
 * @no_display : whether @directory should be displayed.
 *
 * Set the NoDisplay key of @directory.
 */
void
markon_menu_directory_set_no_display (MarkonMenuDirectory *directory,
                                      gboolean             no_display)
{
  g_return_if_fail (MARKON_IS_MENU_DIRECTORY (directory));

  /* Abort if old and new value are equal */
  if (directory->priv->no_display == no_display)
    return;

  /* Assign new value */
  directory->priv->no_display = no_display;

  /* Notify listeners */
  g_object_notify (G_OBJECT (directory), "no-display");
}



/**
 * markon_menu_directory_get_hidden:
 * @directory : a #MarkonMenuDirectory
 *
 * Whether @directory should be hidden.
 * For applications you want to call markon_menu_directory_get_visible().
 *
 * Returns: a the hidden key for @directory.
 */
gboolean
markon_menu_directory_get_hidden (MarkonMenuDirectory *directory)
{
  g_return_val_if_fail (MARKON_IS_MENU_DIRECTORY (directory), FALSE);
  return directory->priv->hidden;
}



/**
 * markon_menu_directory_get_show_in_environment:
 * @directory : a #MarkonMenuDirectory
 *
 * Whether @directory is visible in the current environment
 * which has been set by markon_set_environment().
 * For applications you want to call markon_menu_directory_get_visible().
 *
 * Returns: %TRUE is visible in environment, else %FALSE.
 */
gboolean
markon_menu_directory_get_show_in_environment (MarkonMenuDirectory *directory)
{
  const gchar *env;
  guint        i;
  gboolean     show = TRUE;

  g_return_val_if_fail (MARKON_IS_MENU_DIRECTORY (directory), FALSE);

  /* Determine current environment */
  env = markon_get_environment ();

  /* If no environment has been set, the menu is displayed no matter what
   * OnlyShowIn or NotShowIn contain */
  if (G_UNLIKELY (env == NULL))
    return TRUE;

  /* According to the spec there is either a OnlyShowIn or a NotShowIn list */
  if (G_UNLIKELY (directory->priv->only_show_in != NULL))
    {
      /* Check if your environemnt is in OnlyShowIn list */
      for (i = 0, show = FALSE; !show && directory->priv->only_show_in[i] != NULL; i++)
        if (g_strcmp0 (directory->priv->only_show_in[i], env) == 0)
          show = TRUE;
    }
  else if (G_UNLIKELY (directory->priv->not_show_in != NULL))
    {
      /* Check if your environemnt is in NotShowIn list */
      for (i = 0, show = TRUE; show && directory->priv->not_show_in[i] != NULL; i++)
        if (g_strcmp0 (directory->priv->not_show_in[i], env) == 0)
          show = FALSE;
    }

  return show;
}



/**
 * markon_menu_directory_get_visible:
 * @directory : a #MarkonMenuDirectory
 *
 * Check which runs the following checks:
 * markon_menu_directory_get_show_in_environment(),
 * markon_menu_directory_get_hidden() and
 * markon_menu_directory_get_no_display().
 *
 * Returns: if visible %TRUE, else %FALSE.
 **/
gboolean
markon_menu_directory_get_visible (MarkonMenuDirectory *directory)
{
  g_return_val_if_fail (MARKON_IS_MENU_DIRECTORY (directory), FALSE);

  if (!markon_menu_directory_get_show_in_environment (directory))
    return FALSE;
  else if (markon_menu_directory_get_hidden (directory))
    return FALSE;
  else if (markon_menu_directory_get_no_display (directory))
    return FALSE;

  return TRUE;
}


/**
 * markon_menu_directory_equal:
 * @directory : a #MarkonMenuDirectory
 * @other : a #MarkonMenuDirectory
 *
 * Checks if both directories point to the same file.
 *
 * Returns: if files are equal %TRUE, else %FALSE.
 **/
gboolean
markon_menu_directory_equal (MarkonMenuDirectory *directory,
                             MarkonMenuDirectory *other)
{
  g_return_val_if_fail (MARKON_IS_MENU_DIRECTORY (directory), FALSE);
  g_return_val_if_fail (MARKON_IS_MENU_DIRECTORY (other), FALSE);
  return g_file_equal (directory->priv->file, other->priv->file);
}
