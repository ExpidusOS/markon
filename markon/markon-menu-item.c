/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2006-2010 Jannis Pohlmann <jannis@expidus.org>
 * Copyright (c) 2009-2010 Nick Schermer <nick@expidus.org>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gio/gio.h>
#include <libexpidus1util/libexpidus1util.h>

#include <markon/markon-environment.h>
#include <markon/markon-menu-element.h>
#include <markon/markon-menu-item.h>
#include <markon/markon-menu-item-action.h>
#include <markon/markon-private.h>



/* Property identifiers */
enum
{
  PROP_0,
  PROP_FILE,
  PROP_DESKTOP_ID,
  PROP_REQUIRES_TERMINAL,
  PROP_NO_DISPLAY,
  PROP_STARTUP_NOTIFICATION,
  PROP_NAME,
  PROP_GENERIC_NAME,
  PROP_COMMENT,
  PROP_ICON_NAME,
  PROP_COMMAND,
  PROP_TRY_EXEC,
  PROP_HIDDEN,
  PROP_PATH,
};

/* Signal identifiers */
enum
{
  CHANGED,
  LAST_SIGNAL,
};



static void         markon_menu_item_element_init                    (MarkonMenuElementIface *iface);
static void         markon_menu_item_finalize                        (GObject                *object);
static void         markon_menu_item_get_property                    (GObject                *object,
                                                                      guint                   prop_id,
                                                                      GValue                 *value,
                                                                      GParamSpec             *pspec);
static void         markon_menu_item_set_property                    (GObject                *object,
                                                                      guint                   prop_id,
                                                                      const GValue           *value,
                                                                      GParamSpec             *pspec);
static const gchar *markon_menu_item_get_element_name                (MarkonMenuElement      *element);
static const gchar *markon_menu_item_get_element_comment             (MarkonMenuElement      *element);
static const gchar *markon_menu_item_get_element_icon_name           (MarkonMenuElement      *element);
static gboolean     markon_menu_item_get_element_visible             (MarkonMenuElement      *element);
static gboolean     markon_menu_item_get_element_show_in_environment (MarkonMenuElement      *element);
static gboolean     markon_menu_item_get_element_no_display          (MarkonMenuElement      *element);
static gboolean     markon_menu_item_get_element_equal               (MarkonMenuElement      *element,
                                                                      MarkonMenuElement      *other);
static gboolean     markon_menu_item_lists_equal                     (GList                  *list1,
                                                                      GList                  *list2);



static guint item_signals[LAST_SIGNAL];



struct _MarkonMenuItemPrivate
{
  /* Source file of the menu item */
  GFile      *file;

  /* Desktop file id */
  gchar      *desktop_id;

  /* List of categories */
  GList      *categories;

  /* List of keywords */
  GList      *keywords;

  /* Whether this application requires a terminal to be started in */
  guint       requires_terminal : 1;

  /* Whether this menu item should be hidden */
  guint       no_display : 1;

  /* Whether this application supports startup notification */
  guint       supports_startup_notification : 1;

  /* Name to be displayed for the menu item */
  gchar      *name;

  /* Generic name of the menu item */
  gchar      *generic_name;

  /* Comment/description of the item */
  gchar      *comment;

  /* Command to be executed when the menu item is clicked */
  gchar      *command;

  /* TryExec value */
  gchar      *try_exec;

  /* Menu item icon name */
  gchar      *icon_name;

  /* Environments in which the menu item should be displayed only */
  gchar     **only_show_in;

  /* Environments in which the menu item should be hidden */
  gchar     **not_show_in;

  /* Working directory */
  gchar      *path;

  /* List of application actions of type MarkonMenuItemAction */
  GList      *actions;

  /* Hidden value */
  guint       hidden : 1;

  /* Counter keeping the number of menus which use this item. This works
   * like a reference counter and should be increased / decreased by MarkonMenu
   * items whenever the item is added to or removed from the menu. */
  guint       num_allocated;
};



G_DEFINE_TYPE_WITH_CODE (MarkonMenuItem, markon_menu_item, G_TYPE_OBJECT,
                         G_ADD_PRIVATE (MarkonMenuItem)
                         G_IMPLEMENT_INTERFACE (MARKON_TYPE_MENU_ELEMENT,
                                                markon_menu_item_element_init))



static void
markon_menu_item_class_init (MarkonMenuItemClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = markon_menu_item_finalize;
  gobject_class->get_property = markon_menu_item_get_property;
  gobject_class->set_property = markon_menu_item_set_property;

  /**
   * MarkonMenuItem:file:
   *
   * The #GFile from which the %MarkonMenuItem was loaded.
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
   * MarkonMenuItem:desktop-id:
   *
   * The desktop-file id of this application.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_DESKTOP_ID,
                                   g_param_spec_string ("desktop-id",
                                                        "Desktop-File Id",
                                                        "Desktop-File Id of the application",
                                                        NULL,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

  /**
   * MarkonMenuItem:requires-terminal:
   *
   * Whether this application requires a terinal to be started in.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_REQUIRES_TERMINAL,
                                   g_param_spec_boolean ("requires-terminal",
                                                         "Requires a terminal",
                                                         "Whether this application requires a terminal",
                                                         FALSE,
                                                         G_PARAM_READWRITE |
                                                         G_PARAM_STATIC_STRINGS));

  /**
   * MarkonMenuItem:no-display:
   *
   * Whether this menu item is hidden in menus.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_NO_DISPLAY,
                                   g_param_spec_boolean ("no-display",
                                                         "No Display",
                                                         "Visibility state of the menu item",
                                                         FALSE,
                                                         G_PARAM_READWRITE |
                                                         G_PARAM_STATIC_STRINGS));

  /**
   * MarkonMenuItem:startup-notification:
   *
   * Whether this application supports startup notification.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_STARTUP_NOTIFICATION,
                                   g_param_spec_boolean ("supports-startup-notification",
                                                         "Startup notification",
                                                         "Startup notification support",
                                                         FALSE,
                                                         G_PARAM_READWRITE |
                                                         G_PARAM_STATIC_STRINGS));

  /**
   * MarkonMenuItem:name:
   *
   * Name of the application (will be displayed in menus etc.).
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_NAME,
                                   g_param_spec_string ("name",
                                                        "Name",
                                                        "Name of the application",
                                                        NULL,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

  /**
   * MarkonMenuItem:generic-name:
   *
   * GenericName of the application (will be displayed in menus etc.).
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_GENERIC_NAME,
                                   g_param_spec_string ("generic-name",
                                                        "Generic name",
                                                        "Generic name of the application",
                                                        NULL,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

  /**
   * MarkonMenuItem:comment:
   *
   * Comment/description for the application. To be displayed e.g. in tooltips of
   * GtkMenuItems.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_COMMENT,
                                   g_param_spec_string ("comment",
                                                        "Comment",
                                                        "Comment/description for the application",
                                                        NULL,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

  /**
   * MarkonMenuItem:command:
   *
   * Command to be executed when the menu item is clicked.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_COMMAND,
                                   g_param_spec_string ("command",
                                                        "Command",
                                                        "Application command",
                                                        NULL,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

  /**
   * MarkonMenuItem:try-exec:
   *
   * Path to an executable file on disk used to determine if the program
   * is actually installed. If the path is not an absolute path, the file
   * is looked up in the $PATH environment variable. If the file is not
   * present or if it is not executable, the entry may be ignored (not be
   * used in menus, for example).
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_TRY_EXEC,
                                   g_param_spec_string ("try-exec",
                                                        "TryExec",
                                                        "Command to check if application is installed",
                                                        NULL,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

  /**
   * MarkonMenuItem:icon-name:
   *
   * Name of the icon to be displayed for this menu item.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_ICON_NAME,
                                   g_param_spec_string ("icon-name",
                                                        "Icon name",
                                                        "Name of the application icon",
                                                        NULL,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

 /**
   * MarkonMenuItem:hidden:
   *
   * It means the user deleted (at his level) something that was present
   * (at an upper level, e.g. in the system dirs). It's strictly equivalent
   * to the .desktop file not existing at all, as far as that user is concerned.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_HIDDEN,
                                   g_param_spec_boolean ("hidden",
                                                         "Hidden",
                                                         "Whether the application has been deleted",
                                                          FALSE,
                                                          G_PARAM_READWRITE |
                                                          G_PARAM_STATIC_STRINGS));

 /**
   * MarkonMenuItem:path:
   *
   * Working directory the application should be started in.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_PATH,
                                   g_param_spec_string ("path",
                                                        "Path",
                                                        "Working directory path",
                                                        NULL,
                                                        G_PARAM_READWRITE |
                                                        G_PARAM_STATIC_STRINGS));

  /**
   * MarkonMenuItem::changed:
   * @item : a #MarkonMenuItem.
   *
   * Emitted when #MarkonMenuItem has been reloaded.
   **/
  item_signals[CHANGED] =
    g_signal_new (g_intern_static_string ("changed"),
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (MarkonMenuItemClass, changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
}



static void
markon_menu_item_element_init (MarkonMenuElementIface *iface)
{
  iface->get_name = markon_menu_item_get_element_name;
  iface->get_comment = markon_menu_item_get_element_comment;
  iface->get_icon_name = markon_menu_item_get_element_icon_name;
  iface->get_visible = markon_menu_item_get_element_visible;
  iface->get_show_in_environment = markon_menu_item_get_element_show_in_environment;
  iface->get_no_display = markon_menu_item_get_element_no_display;
  iface->equal = markon_menu_item_get_element_equal;
}



static void
markon_menu_item_init (MarkonMenuItem *item)
{
  item->priv = markon_menu_item_get_instance_private (item);
}



static void
markon_menu_item_finalize (GObject *object)
{
  MarkonMenuItem *item = MARKON_MENU_ITEM (object);

  g_free (item->priv->desktop_id);
  g_free (item->priv->name);
  g_free (item->priv->generic_name);
  g_free (item->priv->comment);
  g_free (item->priv->command);
  g_free (item->priv->try_exec);
  g_free (item->priv->icon_name);
  g_free (item->priv->path);

  g_strfreev (item->priv->only_show_in);
  g_strfreev (item->priv->not_show_in);

  _markon_g_list_free_full (item->priv->categories, g_free);
  _markon_g_list_free_full (item->priv->keywords, g_free);
  _markon_g_list_free_full (item->priv->actions, markon_menu_item_action_unref);

  if (item->priv->file != NULL)
    g_object_unref (G_OBJECT (item->priv->file));

  (*G_OBJECT_CLASS (markon_menu_item_parent_class)->finalize) (object);
}



static void
markon_menu_item_get_property (GObject    *object,
                               guint       prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  MarkonMenuItem *item = MARKON_MENU_ITEM (object);

  switch (prop_id)
    {
    case PROP_FILE:
      g_value_set_object (value, item->priv->file);
      break;

    case PROP_DESKTOP_ID:
      g_value_set_string (value, markon_menu_item_get_desktop_id (item));
      break;

    case PROP_COMMENT:
      g_value_set_string (value, markon_menu_item_get_comment (item));
      break;

    case PROP_REQUIRES_TERMINAL:
      g_value_set_boolean (value, markon_menu_item_requires_terminal (item));
      break;

    case PROP_NO_DISPLAY:
      g_value_set_boolean (value, markon_menu_item_get_no_display (item));
      break;

    case PROP_STARTUP_NOTIFICATION:
      g_value_set_boolean (value, markon_menu_item_supports_startup_notification (item));
      break;

    case PROP_NAME:
      g_value_set_string (value, markon_menu_item_get_name (item));
      break;

    case PROP_GENERIC_NAME:
      g_value_set_string (value, markon_menu_item_get_generic_name (item));
      break;

    case PROP_COMMAND:
      g_value_set_string (value, markon_menu_item_get_command (item));
      break;

    case PROP_ICON_NAME:
      g_value_set_string (value, markon_menu_item_get_icon_name (item));
      break;

    case PROP_TRY_EXEC:
      g_value_set_string (value, markon_menu_item_get_try_exec (item));
      break;

    case PROP_HIDDEN:
      g_value_set_boolean (value, markon_menu_item_get_hidden (item));
      break;

    case PROP_PATH:
      g_value_set_string (value, markon_menu_item_get_path (item));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
markon_menu_item_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
  MarkonMenuItem *item = MARKON_MENU_ITEM (object);

  switch (prop_id)
    {
    case PROP_FILE:
      item->priv->file = g_value_dup_object (value);
      break;

    case PROP_DESKTOP_ID:
      markon_menu_item_set_desktop_id (item, g_value_get_string (value));
      break;

    case PROP_REQUIRES_TERMINAL:
      markon_menu_item_set_requires_terminal (item, g_value_get_boolean (value));
      break;

    case PROP_NO_DISPLAY:
      markon_menu_item_set_no_display (item, g_value_get_boolean (value));
      break;

    case PROP_STARTUP_NOTIFICATION:
      markon_menu_item_set_supports_startup_notification (item, g_value_get_boolean (value));
      break;

    case PROP_NAME:
      markon_menu_item_set_name (item, g_value_get_string (value));
      break;

    case PROP_GENERIC_NAME:
      markon_menu_item_set_generic_name (item, g_value_get_string (value));
      break;

    case PROP_COMMENT:
      markon_menu_item_set_comment (item, g_value_get_string (value));
      break;

    case PROP_COMMAND:
      markon_menu_item_set_command (item, g_value_get_string (value));
      break;

    case PROP_TRY_EXEC:
      markon_menu_item_set_try_exec (item, g_value_get_string (value));
      break;

    case PROP_ICON_NAME:
      markon_menu_item_set_icon_name (item, g_value_get_string (value));
      break;

    case PROP_HIDDEN:
      markon_menu_item_set_hidden (item, g_value_get_boolean (value));
      break;

    case PROP_PATH:
      markon_menu_item_set_path (item, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static gboolean
markon_menu_item_get_element_visible (MarkonMenuElement *element)
{
  MarkonMenuItem  *item;
  const gchar     *try_exec;
  gchar          **mt;
  gboolean         result = TRUE;
  gchar           *command;

  g_return_val_if_fail (MARKON_IS_MENU_ITEM (element), FALSE);

  item = MARKON_MENU_ITEM (element);

  if (markon_menu_item_get_hidden (item)
      || markon_menu_item_get_no_display (item)
      || !markon_menu_item_get_show_in_environment (item))
    return FALSE;

  /* Check the TryExec field */
  try_exec = markon_menu_item_get_try_exec (item);
  if (try_exec != NULL && g_shell_parse_argv (try_exec, NULL, &mt, NULL))
    {
      /* Check if we have an absolute path to an existing file */
      result = g_file_test (mt[0], G_FILE_TEST_EXISTS);

      /* Else, we may have a program in $PATH */
      if (!result)
        {
          command = g_find_program_in_path (mt[0]);
          result = (command != NULL);
          g_free (command);
        }

      /* Cleanup */
      g_strfreev (mt);
    }

  return result;
}



static gboolean
markon_menu_item_get_element_show_in_environment (MarkonMenuElement *element)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (element), FALSE);
  return markon_menu_item_get_show_in_environment (MARKON_MENU_ITEM (element));
}



static gboolean
markon_menu_item_get_element_no_display (MarkonMenuElement *element)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (element), FALSE);
  return markon_menu_item_get_no_display (MARKON_MENU_ITEM (element));
}



static gboolean
markon_menu_item_get_element_equal (MarkonMenuElement *element,
                                    MarkonMenuElement *other)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (element), FALSE);
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (other), FALSE);

  return g_file_equal (MARKON_MENU_ITEM (element)->priv->file,
                       MARKON_MENU_ITEM (other)->priv->file);
}



static const gchar*
markon_menu_item_get_element_name (MarkonMenuElement *element)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (element), NULL);
  return MARKON_MENU_ITEM (element)->priv->name;
}



static const gchar*
markon_menu_item_get_element_comment (MarkonMenuElement *element)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (element), NULL);
  return MARKON_MENU_ITEM (element)->priv->comment;
}



static const gchar*
markon_menu_item_get_element_icon_name (MarkonMenuElement *element)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (element), NULL);
  return MARKON_MENU_ITEM (element)->priv->icon_name;
}



static gboolean
markon_menu_item_lists_equal (GList *list1,
                              GList *list2)
{
  gboolean  element_missing = FALSE;
  GList    *lp;

  if (g_list_length (list1) != g_list_length (list2))
    return FALSE;

  for (lp = list1; !element_missing && lp != NULL; lp = lp->next)
    {
      if (g_list_find_custom (list2, lp->data, (GCompareFunc) g_strcmp0) == NULL)
        element_missing = TRUE;
    }

  return !element_missing;
}



static gchar *
markon_menu_item_url_exec (ExpidusRc *rc)
{
  const gchar *url;
  gchar       *url_exec = NULL;

  /* Support Type=Link items */
  url = expidus_rc_read_entry_untranslated (rc, G_KEY_FILE_DESKTOP_KEY_URL, NULL);
  if (url != NULL)
    url_exec = g_strdup_printf ("endo-open '%s'", url);

  return url_exec;
}


/**
 * markon_menu_item_new: (constructor)
 * @file: a #GFile
 *
 * Returns (transfer full): a new #MarkonMenuItem
 */
MarkonMenuItem *
markon_menu_item_new (GFile *file)
{
  MarkonMenuItem       *item = NULL;
  MarkonMenuItemAction *action = NULL;
  ExpidusRc               *rc;
  GList                *categories = NULL;
  GList                *keywords = NULL;
  gchar                *filename;
  gboolean              terminal;
  gboolean              no_display;
  gboolean              startup_notify;
  gboolean              hidden;
  const gchar          *path;
  const gchar          *name;
  const gchar          *generic_name;
  const gchar          *comment;
  const gchar          *exec;
  const gchar          *try_exec;
  const gchar          *icon;
  gchar                *action_group;
  gchar               **mt;
  gchar               **str_list;
  gchar                *url_exec = NULL;

  g_return_val_if_fail (G_IS_FILE (file), NULL);
  g_return_val_if_fail (g_file_is_native (file), NULL);

  /* Open the rc file */
  filename = g_file_get_path (file);
  rc = expidus_rc_simple_open (filename, TRUE);
  g_free (filename);
  if (G_UNLIKELY (rc == NULL))
    return NULL;

  expidus_rc_set_group (rc, G_KEY_FILE_DESKTOP_GROUP);

  /* Parse name and exec command */
  name = expidus_rc_read_entry (rc, G_KEY_FILE_DESKTOP_KEY_NAME, NULL);
  exec = expidus_rc_read_entry_untranslated (rc, G_KEY_FILE_DESKTOP_KEY_EXEC, NULL);

  /* Support Type=Link items */
  if (G_UNLIKELY (exec == NULL))
    exec = url_exec = markon_menu_item_url_exec (rc);

  /* Validate Name and Exec fields */
  if (G_LIKELY (exec != NULL && name != NULL))
    {
      /* Determine other application properties */
      generic_name = expidus_rc_read_entry (rc, G_KEY_FILE_DESKTOP_KEY_GENERIC_NAME, NULL);
      comment = expidus_rc_read_entry (rc, G_KEY_FILE_DESKTOP_KEY_COMMENT, NULL);
      try_exec = expidus_rc_read_entry_untranslated (rc, G_KEY_FILE_DESKTOP_KEY_TRY_EXEC, NULL);
      icon = expidus_rc_read_entry_untranslated (rc, G_KEY_FILE_DESKTOP_KEY_ICON, NULL);
      path = expidus_rc_read_entry_untranslated (rc, G_KEY_FILE_DESKTOP_KEY_PATH, NULL);
      terminal = expidus_rc_read_bool_entry (rc, G_KEY_FILE_DESKTOP_KEY_TERMINAL, FALSE);
      no_display = expidus_rc_read_bool_entry (rc, G_KEY_FILE_DESKTOP_KEY_NO_DISPLAY, FALSE);
      startup_notify = expidus_rc_read_bool_entry (rc, G_KEY_FILE_DESKTOP_KEY_STARTUP_NOTIFY, FALSE)
                       || expidus_rc_read_bool_entry (rc, "X-KDE-StartupNotify", FALSE);
      hidden = expidus_rc_read_bool_entry (rc, G_KEY_FILE_DESKTOP_KEY_HIDDEN, FALSE);

      /* Allocate a new menu item instance */
      item = g_object_new (MARKON_TYPE_MENU_ITEM,
                           "file", file,
                           "command", exec,
                           "try-exec", try_exec,
                           "name", name,
                           "generic-name", generic_name,
                           "comment", comment,
                           "icon-name", icon,
                           "requires-terminal", terminal,
                           "no-display", no_display,
                           "supports-startup-notification", startup_notify,
                           "path", path,
                           "hidden", hidden,
                           NULL);

      /* Determine the categories this application should be shown in */
      str_list = expidus_rc_read_list_entry (rc, G_KEY_FILE_DESKTOP_KEY_CATEGORIES, ";");
      if (G_LIKELY (str_list != NULL))
        {
          for (mt = str_list; *mt != NULL; ++mt)
            {
              /* Try to steal the values */
              if (**mt != '\0')
                categories = g_list_prepend (categories, *mt);
              else
                g_free (*mt);
            }

          /* Cleanup */
          g_free (str_list);

          /* Assign categories list to the menu item */
          markon_menu_item_set_categories (item, categories);
        }

      /* Determine the keywords this application should be shown in */
      str_list = expidus_rc_read_list_entry (rc, G_KEY_FILE_DESKTOP_KEY_KEYWORDS, ";");
      if (G_LIKELY (str_list != NULL))
        {
          for (mt = str_list; *mt != NULL; ++mt)
            {
              /* Try to steal the values */
              if (**mt != '\0')
                keywords = g_list_prepend (keywords, *mt);
              else
                g_free (*mt);
            }

          /* Cleanup */
          g_free (str_list);

          /* Assign keywords list to the menu item */
          markon_menu_item_set_keywords (item, keywords);
        }

      /* Set the rest of the private data directly */
      item->priv->only_show_in = expidus_rc_read_list_entry (rc, G_KEY_FILE_DESKTOP_KEY_ONLY_SHOW_IN, ";");
      item->priv->not_show_in = expidus_rc_read_list_entry (rc, G_KEY_FILE_DESKTOP_KEY_NOT_SHOW_IN, ";");

      /* Determine this application actions */
      str_list = expidus_rc_read_list_entry (rc, G_KEY_FILE_DESKTOP_KEY_ACTIONS, ";");
      if (G_LIKELY (str_list != NULL))
        {
          for (mt = str_list; *mt != NULL; ++mt)
            {
              if (**mt != '\0')
                {
                  /* Set current desktop action group */
                  action_group = g_strdup_printf ("Desktop Action %s", *mt);
                  expidus_rc_set_group (rc, action_group);

                  /* Parse name and exec command */
                  name = expidus_rc_read_entry (rc, G_KEY_FILE_DESKTOP_KEY_NAME, NULL);
                  exec = expidus_rc_read_entry_untranslated (rc, G_KEY_FILE_DESKTOP_KEY_EXEC, NULL);
                  icon = expidus_rc_read_entry_untranslated (rc, G_KEY_FILE_DESKTOP_KEY_ICON, NULL);

                  /* Validate Name and Exec fields, icon is optional */
                  if (G_LIKELY (exec != NULL && name != NULL))
                    {
                      /* Allocate a new action instance */
                      action = g_object_new (MARKON_TYPE_MENU_ITEM_ACTION,
                                             "name", name,
                                             "command", exec,
                                             "icon-name", icon,
                                             NULL);

                      markon_menu_item_set_action (item, *mt, action);
                    }

                  g_free (action_group);
                }
            }

          /* Cleanup */
          g_strfreev (str_list);
        }

      else
        {
          str_list = expidus_rc_read_list_entry (rc, "X-Ayatana-Desktop-Shortcuts", ";");
          if (G_LIKELY (str_list != NULL))
            {
              for (mt = str_list; *mt != NULL; ++mt)
                {
                  if (**mt != '\0')
                    {
                      action_group = g_strdup_printf ("%s Shortcut Group", *mt);
                      expidus_rc_set_group (rc, action_group);

                      name = expidus_rc_read_entry (rc, G_KEY_FILE_DESKTOP_KEY_NAME, NULL);
                      exec = expidus_rc_read_entry_untranslated (rc, G_KEY_FILE_DESKTOP_KEY_EXEC, NULL);
                      icon = expidus_rc_read_entry_untranslated (rc, G_KEY_FILE_DESKTOP_KEY_ICON, NULL);

                      /* Validate Name and Exec fields, icon is optional */
                      if (G_LIKELY (exec != NULL && name != NULL))
                        {
                          action = g_object_new (MARKON_TYPE_MENU_ITEM_ACTION,
                                                 "name", name,
                                                 "command", exec,
                                                 "icon-name", icon,
                                                 NULL);

                          markon_menu_item_set_action (item, *mt, action);
                        }

                      g_free (action_group);
                    }
                }

              g_strfreev (str_list);
            }
        }
    }

  /* Cleanup */
  expidus_rc_close (rc);
  g_free (url_exec);

  return item;
}


/**
 * markon_menu_item_new_for_path: (constructor)
 * @filename: (type filename): name of a file
 *
 * Returns: (transfer full): a new #MarkonMenuItem
 */
MarkonMenuItem *
markon_menu_item_new_for_path (const gchar *filename)
{
  GFile          *file;
  MarkonMenuItem *item;

  g_return_val_if_fail (filename != NULL, NULL);

  file = g_file_new_for_path (filename);
  item = markon_menu_item_new (file);
  g_object_unref (G_OBJECT (file));

  return item;
}


/**
 * markon_menu_item_new_for_uri: (constructor)
 * @uri: a given URI
 *
 * Returns: (transfer full): a new #MarkonMenuItem
 */
MarkonMenuItem *
markon_menu_item_new_for_uri (const gchar *uri)
{
  GFile          *file;
  MarkonMenuItem *item;

  g_return_val_if_fail (uri != NULL, NULL);

  file = g_file_new_for_uri (uri);
  item = markon_menu_item_new (file);
  g_object_unref (G_OBJECT (file));

  return item;
}



gboolean
markon_menu_item_reload (MarkonMenuItem  *item,
                         gboolean        *affects_the_outside,
                         GError         **error)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  return markon_menu_item_reload_from_file (item, item->priv->file, affects_the_outside, error);
}



gboolean
markon_menu_item_reload_from_file (MarkonMenuItem  *item,
                                   GFile           *file,
                                   gboolean        *affects_the_outside,
                                   GError         **error)
{
  ExpidusRc               *rc;
  MarkonMenuItemAction *action = NULL;
  gboolean              boolean;
  GList                *categories = NULL;
  GList                *old_categories = NULL;
  GList                *keywords = NULL;
  GList                *old_keywords = NULL;
  GList                *lp;
  gchar               **mt;
  gchar               **str_list;
  const gchar          *string;
  const gchar          *name;
  const gchar          *exec;
  const gchar          *icon;
  gchar                *filename;
  gchar                *action_group;
  gchar                *url_exec = NULL;

  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), FALSE);
  g_return_val_if_fail (G_IS_FILE (file), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);
  g_return_val_if_fail (g_file_is_native (file), FALSE);

  /* Open the rc file */
  filename = g_file_get_path (file);
  rc = expidus_rc_simple_open (filename, TRUE);
  g_free (filename);
  if (G_UNLIKELY (rc == NULL))
    return FALSE;

  expidus_rc_set_group (rc, G_KEY_FILE_DESKTOP_GROUP);

  /* Check if there is a name and exec key */
  name = expidus_rc_read_entry (rc, G_KEY_FILE_DESKTOP_KEY_NAME, NULL);
  exec = expidus_rc_read_entry_untranslated (rc, G_KEY_FILE_DESKTOP_KEY_EXEC, NULL);

  /* Support Type=Link items */
  if (G_UNLIKELY (exec == NULL))
    exec = url_exec = markon_menu_item_url_exec (rc);

  if (G_UNLIKELY (name == NULL || exec == NULL))
    {
      g_set_error_literal (error, G_KEY_FILE_ERROR,
                           G_KEY_FILE_ERROR_KEY_NOT_FOUND,
                           "Either the name or exec key was not defined.");
      expidus_rc_close (rc);

      return FALSE;
    }

  /* Queue property notifications */
  g_object_freeze_notify (G_OBJECT (item));

  /* Set the new file if needed */
  if (!g_file_equal (file, item->priv->file))
    {
      if (G_LIKELY (item->priv->file != NULL))
        g_object_unref (G_OBJECT (item->priv->file));
      item->priv->file = G_FILE (g_object_ref (G_OBJECT (file)));

      g_object_notify (G_OBJECT (item), "file");
    }

  /* Update properties */
  markon_menu_item_set_name (item, name);

  markon_menu_item_set_command (item, exec);

  string = expidus_rc_read_entry (rc, G_KEY_FILE_DESKTOP_KEY_GENERIC_NAME, NULL);
  markon_menu_item_set_generic_name (item, string);

  string = expidus_rc_read_entry (rc, G_KEY_FILE_DESKTOP_KEY_COMMENT, NULL);
  markon_menu_item_set_comment (item, string);

  string = expidus_rc_read_entry_untranslated (rc, G_KEY_FILE_DESKTOP_KEY_TRY_EXEC, NULL);
  markon_menu_item_set_try_exec (item, string);

  string = expidus_rc_read_entry_untranslated (rc, G_KEY_FILE_DESKTOP_KEY_ICON, NULL);
  markon_menu_item_set_icon_name (item, string);

  string = expidus_rc_read_entry_untranslated (rc, G_KEY_FILE_DESKTOP_KEY_PATH, NULL);
  markon_menu_item_set_path (item, string);

  boolean = expidus_rc_read_bool_entry (rc, G_KEY_FILE_DESKTOP_KEY_TERMINAL, FALSE);
  markon_menu_item_set_requires_terminal (item, boolean);

  boolean = expidus_rc_read_bool_entry (rc, G_KEY_FILE_DESKTOP_KEY_NO_DISPLAY, FALSE);
  markon_menu_item_set_no_display (item, boolean);

  boolean = expidus_rc_read_bool_entry (rc, G_KEY_FILE_DESKTOP_KEY_STARTUP_NOTIFY, FALSE)
            || expidus_rc_read_bool_entry (rc, "X-KDE-StartupNotify", FALSE);
  markon_menu_item_set_supports_startup_notification (item, boolean);

  boolean = expidus_rc_read_bool_entry (rc, G_KEY_FILE_DESKTOP_KEY_HIDDEN, FALSE);
  markon_menu_item_set_hidden (item, boolean);

  if (affects_the_outside != NULL)
    {
      /* create a deep copy the old categories list */
      old_categories = g_list_copy (item->priv->categories);
      for (lp = old_categories; lp != NULL; lp = lp->next)
        lp->data = g_strdup (lp->data);
    }

  /* Determine the categories this application should be shown in */
  str_list = expidus_rc_read_list_entry (rc, G_KEY_FILE_DESKTOP_KEY_CATEGORIES, ";");
  if (G_LIKELY (str_list != NULL))
    {
      for (mt = str_list; *mt != NULL; ++mt)
        {
          /* Try to steal the values */
          if (**mt != '\0')
            categories = g_list_prepend (categories, *mt);
          else
            g_free (*mt);
        }

      /* Cleanup */
      g_free (str_list);

      /* Assign categories list to the menu item */
      markon_menu_item_set_categories (item, categories);
    }
  else
    {
      /* Assign empty categories list to the menu item */
      markon_menu_item_set_categories (item, NULL);
    }

  if (affects_the_outside != NULL)
    {
      if (!markon_menu_item_lists_equal (old_categories, categories))
        *affects_the_outside = TRUE;

      _markon_g_list_free_full (old_categories, g_free);
    }


  if (affects_the_outside != NULL)
    {
      /* create a deep copy the old keywords list */
      old_keywords = g_list_copy (item->priv->keywords);
      for (lp = old_keywords; lp != NULL; lp = lp->next)
        lp->data = g_strdup (lp->data);
    }

  /* Determine the keywords this application should be shown in */
  str_list = expidus_rc_read_list_entry (rc, G_KEY_FILE_DESKTOP_KEY_KEYWORDS, ";");
  if (G_LIKELY (str_list != NULL))
    {
      for (mt = str_list; *mt != NULL; ++mt)
        {
          /* Try to steal the values */
          if (**mt != '\0')
            keywords = g_list_prepend (keywords, *mt);
          else
            g_free (*mt);
        }

      /* Cleanup */
      g_free (str_list);

      /* Assign keywords list to the menu item */
      markon_menu_item_set_keywords (item, keywords);
    }
  else
    {
      /* Assign empty keywords list to the menu item */
      markon_menu_item_set_keywords (item, NULL);
    }

  if (affects_the_outside != NULL)
    {
      if (!markon_menu_item_lists_equal (old_keywords, keywords))
        *affects_the_outside = TRUE;

      _markon_g_list_free_full (old_keywords, g_free);
    }

  /* Set the rest of the private data directly */
  item->priv->only_show_in = expidus_rc_read_list_entry (rc, G_KEY_FILE_DESKTOP_KEY_ONLY_SHOW_IN, ";");
  item->priv->not_show_in = expidus_rc_read_list_entry (rc, G_KEY_FILE_DESKTOP_KEY_NOT_SHOW_IN, ";");

  /* Update application actions */
  _markon_g_list_free_full (item->priv->actions, markon_menu_item_action_unref);
  item->priv->actions = NULL;

  str_list = expidus_rc_read_list_entry (rc, G_KEY_FILE_DESKTOP_KEY_ACTIONS, ";");
  if (G_LIKELY (str_list != NULL))
    {
      for (mt = str_list; *mt != NULL; ++mt)
        {
          if (**mt != '\0')
            {
              /* Set current desktop action group */
              action_group = g_strdup_printf ("Desktop Action %s", *mt);
              expidus_rc_set_group (rc, action_group);

              /* Parse name and exec command */
              name = expidus_rc_read_entry (rc, G_KEY_FILE_DESKTOP_KEY_NAME, NULL);
              exec = expidus_rc_read_entry_untranslated (rc, G_KEY_FILE_DESKTOP_KEY_EXEC, NULL);
              icon = expidus_rc_read_entry_untranslated (rc, G_KEY_FILE_DESKTOP_KEY_ICON, NULL);

              /* Validate Name and Exec fields, icon is optional */
              if (G_LIKELY (exec != NULL && name != NULL))
                {
                  /* Allocate a new action instance */
                  action = g_object_new (MARKON_TYPE_MENU_ITEM_ACTION,
                                         "name", name,
                                         "command", exec,
                                         "icon-name", icon,
                                         NULL);

                  markon_menu_item_set_action (item, *mt, action);
                }
              g_free (action_group);
            }
          else
            g_free (*mt);
        }

      /* Cleanup */
      g_free (str_list);
    }

  else
    {
      str_list = expidus_rc_read_list_entry (rc, "X-Ayatana-Desktop-Shortcuts", ";");
      if (G_LIKELY (str_list != NULL))
        {
          for (mt = str_list; *mt != NULL; ++mt)
            {
              if (**mt != '\0')
                {
                  action_group = g_strdup_printf ("%s Shortcut Group", *mt);
                  expidus_rc_set_group (rc, action_group);

                  name = expidus_rc_read_entry (rc, G_KEY_FILE_DESKTOP_KEY_NAME, NULL);
                  exec = expidus_rc_read_entry_untranslated (rc, G_KEY_FILE_DESKTOP_KEY_EXEC, NULL);
                  icon = expidus_rc_read_entry_untranslated (rc, G_KEY_FILE_DESKTOP_KEY_ICON, NULL);

                  /* Validate Name and Exec fields, icon is optional */
                  if (G_LIKELY (exec != NULL && name != NULL))
                    {
                      action = g_object_new (MARKON_TYPE_MENU_ITEM_ACTION,
                                             "name", name,
                                             "command", exec,
                                             "icon-name", icon,
                                             NULL);

                      markon_menu_item_set_action (item, *mt, action);
                    }

                  g_free (action_group);
                }
              else
                g_free (*mt);
            }

          g_free (str_list);
        }
    }

  /* Flush property notifications */
  g_object_thaw_notify (G_OBJECT (item));

  /* Emit signal to everybody knows we reloaded the file */
  g_signal_emit (G_OBJECT (item), item_signals[CHANGED], 0);

  expidus_rc_close (rc);
  g_free (url_exec);

  return TRUE;
}



/**
 * markon_menu_item_get_file:
 * @item: A #MarkonMenuItem
 *
 * Get the #GFile for @item. The returned object should be
 * unreffed with g_object_unref() when no longer needed.
 *
 * Returns: (transfer full): a #GFile.
 */
GFile *
markon_menu_item_get_file (MarkonMenuItem *item)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), NULL);
  return g_object_ref (item->priv->file);
}


gchar *
markon_menu_item_get_uri (MarkonMenuItem *item)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), NULL);
  return g_file_get_uri (item->priv->file);
}



const gchar *
markon_menu_item_get_desktop_id (MarkonMenuItem *item)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), NULL);
  return item->priv->desktop_id;
}



void
markon_menu_item_set_desktop_id (MarkonMenuItem *item,
                                 const gchar    *desktop_id)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));
  g_return_if_fail (desktop_id != NULL);

  /* Abort if old and new desktop_id are equal */
  if (g_strcmp0 (item->priv->desktop_id, desktop_id) == 0)
    return;

  /* Assign the new desktop_id */
  g_free (item->priv->desktop_id);
  item->priv->desktop_id = g_strdup (desktop_id);

  /* Notify listeners */
  g_object_notify (G_OBJECT (item), "desktop-id");
}


/**
 * markon_menu_item_get_categories:
 * @item: a #MarkonMenuItem
 *
 * Returns list of categories
 *
 * Returns: (element-type utf8) (transfer full):
 */
GList*
markon_menu_item_get_categories (MarkonMenuItem *item)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), NULL);
  return item->priv->categories;
}


/**
 * markon_menu_item_set_categories:
 * @item: a #MarkonMenuItem
 * @categories: (element-type utf8): list of categories
 */
void
markon_menu_item_set_categories (MarkonMenuItem *item,
                                 GList          *categories)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));

  /* Abort if lists are equal */
  if (G_UNLIKELY (item->priv->categories == categories))
    return;

  /* Free old list */
  _markon_g_list_free_full (item->priv->categories, g_free);

  /* Assign new list */
  item->priv->categories = categories;
}


/**
 * markon_menu_item_get_keywords:
 * @item: a #MarkonMenuItem
 *
 * Returns: (element-type utf8) (transfer full):
 */
GList*
markon_menu_item_get_keywords (MarkonMenuItem *item)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), NULL);
  return item->priv->keywords;
}


/**
 * markon_menu_item_set_keywords:
 * @item: a #MarkonMenuItem
 * @keywords: (element-type utf8): list of keywords
 */
void
markon_menu_item_set_keywords (MarkonMenuItem *item,
                               GList          *keywords)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));

  /* Abort if lists are equal */
  if (G_UNLIKELY (item->priv->keywords == keywords))
    return;

  /* Free old list */
  _markon_g_list_free_full (item->priv->keywords, g_free);

  /* Assign new list */
  item->priv->keywords = keywords;
}


const gchar*
markon_menu_item_get_command (MarkonMenuItem *item)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), NULL);
  return item->priv->command;
}


void
markon_menu_item_set_command (MarkonMenuItem *item,
                              const gchar    *command)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));
  g_return_if_fail (command != NULL);

  /* Abort if old and new command are equal */
  if (g_strcmp0 (item->priv->command, command) == 0)
    return;

  /* Assign new command */
  g_free (item->priv->command);
  item->priv->command = g_strdup (command);

  /* Notify listeners */
  g_object_notify (G_OBJECT (item), "command");
}



const gchar*
markon_menu_item_get_try_exec (MarkonMenuItem *item)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), NULL);
  return item->priv->try_exec;
}



void
markon_menu_item_set_try_exec (MarkonMenuItem *item,
                               const gchar    *try_exec)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));

  /* Abort if old and new try_exec are equal */
  if (g_strcmp0 (item->priv->try_exec, try_exec) == 0)
    return;

  /* Assign new try_exec */
  g_free (item->priv->try_exec);
  item->priv->try_exec = g_strdup (try_exec);

  /* Notify listeners */
  g_object_notify (G_OBJECT (item), "try-exec");
}



const gchar*
markon_menu_item_get_name (MarkonMenuItem *item)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), NULL);
  return item->priv->name;
}



void
markon_menu_item_set_name (MarkonMenuItem *item,
                           const gchar    *name)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));
  g_return_if_fail (g_utf8_validate (name, -1, NULL));

  /* Abort if old and new name are equal */
  if (g_strcmp0 (item->priv->name, name) == 0)
    return;

  /* Assign new name */
  g_free (item->priv->name);
  item->priv->name = g_strdup (name);

  /* Notify listeners */
  g_object_notify (G_OBJECT (item), "name");
}



const gchar*
markon_menu_item_get_generic_name (MarkonMenuItem *item)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), NULL);
  return item->priv->generic_name;
}



void
markon_menu_item_set_generic_name (MarkonMenuItem *item,
                                   const gchar    *generic_name)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));
  g_return_if_fail (generic_name == NULL || g_utf8_validate (generic_name, -1, NULL));

  /* Abort if old and new generic name are equal */
  if (g_strcmp0 (item->priv->generic_name, generic_name) == 0)
    return;

  /* Assign new generic_name */
  g_free (item->priv->generic_name);
  item->priv->generic_name = g_strdup (generic_name);

  /* Notify listeners */
  g_object_notify (G_OBJECT (item), "generic-name");
}



const gchar*
markon_menu_item_get_comment (MarkonMenuItem *item)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), NULL);
  return item->priv->comment;
}



void
markon_menu_item_set_comment (MarkonMenuItem *item,
                              const gchar    *comment)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));
  g_return_if_fail (comment == NULL || g_utf8_validate (comment, -1, NULL));

  /* Abort if old and new comment are equal */
  if (g_strcmp0 (item->priv->comment, comment) == 0)
    return;

  /* Assign new comment */
  g_free (item->priv->comment);
  item->priv->comment = g_strdup (comment);

  /* Notify listeners */
  g_object_notify (G_OBJECT (item), "comment");
}



const gchar*
markon_menu_item_get_icon_name (MarkonMenuItem *item)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), NULL);
  return item->priv->icon_name;
}



void
markon_menu_item_set_icon_name (MarkonMenuItem *item,
                                const gchar    *icon_name)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));

  /* Abort if old and new icon name are equal */
  if (g_strcmp0 (item->priv->icon_name, icon_name) == 0)
    return;

  /* Assign new icon name */
  g_free (item->priv->icon_name);
  item->priv->icon_name = g_strdup (icon_name);

  /* Notify listeners */
  g_object_notify (G_OBJECT (item), "icon-name");
}



const gchar*
markon_menu_item_get_path (MarkonMenuItem *item)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), NULL);
  return item->priv->path;
}



void
markon_menu_item_set_path (MarkonMenuItem *item,
                           const gchar    *path)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));

  /* Abort if old and new path are equal */
  if (g_strcmp0 (item->priv->path, path) == 0)
    return;

  /* Assign new path */
  g_free (item->priv->path);
  item->priv->path = g_strdup (path);

  /* Notify listeners */
  g_object_notify (G_OBJECT (item), "path");
}



gboolean
markon_menu_item_get_hidden (MarkonMenuItem *item)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), TRUE);
  return item->priv->hidden;
}



void
markon_menu_item_set_hidden (MarkonMenuItem *item,
                             gboolean        hidden)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));

  /* Abort if old and new value are equal */
  if (item->priv->hidden == hidden)
    return;

  /* Assign new value */
  item->priv->hidden = !!hidden;

  /* Notify listeners */
  g_object_notify (G_OBJECT (item), "hidden");
}



gboolean
markon_menu_item_requires_terminal (MarkonMenuItem *item)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), FALSE);
  return item->priv->requires_terminal;
}



void
markon_menu_item_set_requires_terminal (MarkonMenuItem *item,
                                        gboolean        requires_terminal)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));

  /* Abort if old and new value are equal */
  if (item->priv->requires_terminal == requires_terminal)
    return;

  /* Assign new value */
  item->priv->requires_terminal = !!requires_terminal;

  /* Notify listeners */
  g_object_notify (G_OBJECT (item), "requires-terminal");
}



gboolean
markon_menu_item_get_no_display (MarkonMenuItem *item)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), FALSE);
  return item->priv->no_display;
}



void
markon_menu_item_set_no_display (MarkonMenuItem *item,
                                 gboolean        no_display)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));

  /* Abort if old and new value are equal */
  if (item->priv->no_display == no_display)
    return;

  /* Assign new value */
  item->priv->no_display = !!no_display;

  /* Notify listeners */
  g_object_notify (G_OBJECT (item), "no-display");
}



gboolean
markon_menu_item_supports_startup_notification (MarkonMenuItem *item)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), FALSE);
  return item->priv->supports_startup_notification;
}



void
markon_menu_item_set_supports_startup_notification (MarkonMenuItem *item,
                                                    gboolean        supports_startup_notification)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));

  /* Abort if old and new value are equal */
  if (item->priv->supports_startup_notification == supports_startup_notification)
    return;

  /* Assign new value */
  item->priv->supports_startup_notification = !!supports_startup_notification;

  /* Notify listeners */
  g_object_notify (G_OBJECT (item), "supports-startup-notification");
}



gboolean
markon_menu_item_has_category (MarkonMenuItem *item,
                               const gchar    *category)
{
  GList   *iter;
  gboolean found = FALSE;

  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), FALSE);
  g_return_val_if_fail (category != NULL, FALSE);

  for (iter = item->priv->categories; !found && iter != NULL; iter = g_list_next (iter))
    if (g_strcmp0 (iter->data, category) == 0)
      found = TRUE;

  return found;
}



gboolean
markon_menu_item_has_keyword (MarkonMenuItem *item,
                              const gchar    *keyword)
{
  GList   *iter;
  gboolean found = FALSE;

  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), FALSE);
  g_return_val_if_fail (keyword != NULL, FALSE);

  for (iter = item->priv->keywords; !found && iter != NULL; iter = g_list_next (iter))
    if (g_strcmp0 (iter->data, keyword) == 0)
      found = TRUE;

  return found;
}


/**
 * markon_menu_item_get_actions:
 * @item: a #MarkonMenuItem
 *
 * Returns: (element-type MarkonMenuItemAction) (transfer full):
 */
GList *
markon_menu_item_get_actions (MarkonMenuItem *item)
{
  GList                *action_names = NULL;
  GList                *iter;
  MarkonMenuItemAction *action;

  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), NULL);

  for (iter = item->priv->actions; iter != NULL ; iter = g_list_next (iter))
    {
      action = MARKON_MENU_ITEM_ACTION (iter->data);
      action_names = g_list_prepend (action_names, (gchar*)markon_menu_item_action_get_name (action));
    }
  action_names = g_list_reverse (action_names);

  return action_names;
}


/**
 * markon_menu_item_get_action:
 * @item: a #MarkonMenuItem
 * @action_name:
 *
 * Returns: (nullable) (transfer full): a #MarkonMenuItemAction
 */
MarkonMenuItemAction *
markon_menu_item_get_action (MarkonMenuItem *item,
                             const gchar    *action_name)
{
  GList                *iter;
  MarkonMenuItemAction *action;

  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), NULL);
  g_return_val_if_fail (action_name != NULL, NULL);

  for (iter = item->priv->actions; iter != NULL; iter = g_list_next (iter))
    {
      action = MARKON_MENU_ITEM_ACTION (iter->data);
      if (g_strcmp0 (markon_menu_item_action_get_name (action), action_name) == 0)
        return (action);
    }

  return NULL;
}




void
markon_menu_item_set_action (MarkonMenuItem       *item,
                             const gchar          *action_name,
                             MarkonMenuItemAction *action)
{
  GList                *iter;
  MarkonMenuItemAction *old_action;
  gboolean             found = FALSE;

  g_return_if_fail (MARKON_IS_MENU_ITEM (item));
  g_return_if_fail (MARKON_IS_MENU_ITEM_ACTION (action));

  /* If action name is found in list, then insert new action into the list and
   * remove old action */
  for (iter = item->priv->actions; !found && iter != NULL; iter = g_list_next (iter))
    {
      old_action = MARKON_MENU_ITEM_ACTION (iter->data);
      if (g_strcmp0 (markon_menu_item_action_get_name (old_action), action_name) == 0)
        {
           /* Release reference on action currently stored at action name */
           markon_menu_item_action_unref (old_action);

           /* Replace action in list at action name and grab a reference */
           iter->data = action;
           markon_menu_item_action_ref (action);

           /* Set flag that action was found */
           found = TRUE;
        }
    }

  /* If action name was not found in list, then simply add it to list */
  if (found == FALSE)
    {
      /* Add action to list and grab a reference */
      item->priv->actions=g_list_append (item->priv->actions, action);
      markon_menu_item_action_ref (action);
    }
}



gboolean
markon_menu_item_has_action (MarkonMenuItem  *item,
                             const gchar     *action_name)
{
  GList                *iter;
  MarkonMenuItemAction *action;
  gboolean             found = FALSE;

  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), FALSE);
  g_return_val_if_fail (action_name != NULL, FALSE);

  for (iter = item->priv->actions; !found && iter != NULL; iter = g_list_next (iter))
    {
      action = MARKON_MENU_ITEM_ACTION (iter->data);
      if (g_strcmp0 (markon_menu_item_action_get_name (action), action_name) == 0)
        found = TRUE;
    }

  return found;
}



gboolean
markon_menu_item_get_show_in_environment (MarkonMenuItem *item)
{
  const gchar *env;
  guint        i, j;
  gboolean     show = TRUE;
  gchar**      path = NULL;

  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), FALSE);

  /* Determine current environment */
  env = markon_get_environment ();

  /* If no environment has been set, the menu is displayed no matter what
   * OnlyShowIn or NotShowIn contain */
  if (G_UNLIKELY (env == NULL))
    return TRUE;

  /* According to the spec there is either a OnlyShowIn or a NotShowIn list
   * The environment can be multiple Desktop Names separated by a colons */
  if (G_UNLIKELY (item->priv->only_show_in != NULL))
    {
      /* Check if your environemnt is in OnlyShowIn list */
      show = FALSE;
      path = g_strsplit(env, ":", 0);
      for (j = 0; path[j] != NULL; j++)
        for (i = 0; !show && item->priv->only_show_in[i] != NULL; i++)
          if (g_strcmp0 (item->priv->only_show_in[i], path[j]) == 0)
            show = TRUE;
      g_strfreev(path);
    }
  else if (G_UNLIKELY (item->priv->not_show_in != NULL))
    {
      /* Check if your environemnt is in NotShowIn list */
      show = TRUE;
      path = g_strsplit(env, ":", 0);
      for (j = 0; path[j] != NULL; j++)
        for (i = 0; show && item->priv->not_show_in[i] != NULL; i++)
          if (g_strcmp0 (item->priv->not_show_in[i], path[j]) == 0)
            show = FALSE;
      g_strfreev(path);
    }

  return show;
}



gboolean
markon_menu_item_only_show_in_environment (MarkonMenuItem *item)
{
  const gchar *env;
  guint        i, j;
  gboolean     show = FALSE;
  gchar**      path = NULL;

  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), FALSE);

  /* Determine current environment */
  env = markon_get_environment ();

  /* If no environment has been set, the contents of OnlyShowIn don't matter */
  if (G_LIKELY (env == NULL))
    return FALSE;

  /* Check if we have an OnlyShowIn list */
  if (G_UNLIKELY (item->priv->only_show_in != NULL))
    {
      /* Check if your environemnt is in OnlyShowIn list */
      show = FALSE;
      path = g_strsplit(env, ":", 0);
      for (j= 0; path[j] != NULL; j++)
        for (i = 0; !show && item->priv->only_show_in[i] != NULL; i++)
          if (g_strcmp0 (item->priv->only_show_in[i], path[j]) == 0)
            show = TRUE;
    }

  return show;
}



void
markon_menu_item_ref (MarkonMenuItem *item)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));

  /* Increment the allocation counter */
  markon_menu_item_increment_allocated (item);

  /* Grab a reference on the object */
  g_object_ref (G_OBJECT (item));
}



void
markon_menu_item_unref (MarkonMenuItem *item)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));

  markon_menu_item_decrement_allocated (item);

  /* Decrement the reference counter */
  g_object_unref (G_OBJECT (item));
}



gint
markon_menu_item_get_allocated (MarkonMenuItem *item)
{
  g_return_val_if_fail (MARKON_IS_MENU_ITEM (item), FALSE);
  return item->priv->num_allocated;
}



void
markon_menu_item_increment_allocated (MarkonMenuItem *item)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));
  item->priv->num_allocated++;
}



void
markon_menu_item_decrement_allocated (MarkonMenuItem *item)
{
  g_return_if_fail (MARKON_IS_MENU_ITEM (item));

  if (item->priv->num_allocated > 0)
    item->priv->num_allocated--;
}
