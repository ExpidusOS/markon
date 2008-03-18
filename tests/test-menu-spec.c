/* $Id$ */
/*-
 * vi:set et ai sts=2 sw=2 cindent:
 *
 * Copyright (c) 2007 Jannis Pohlmann <jannis@xfce.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <glib/gprintf.h>

#include <libxfce4util/libxfce4util.h>
#include <libxfce4menu/libxfce4menu.h>



void
print_menu (XfceMenu *menu, gboolean is_root)
{
  XfceMenuDirectory *directory;
  GSList            *menus;
  GSList            *items;
  GSList            *iter;
  const gchar       *name;

  /* Determine menu name */
  directory = xfce_menu_get_directory (menu);
  name = is_root ? "" : (directory == NULL ? xfce_menu_get_name (menu) : xfce_menu_directory_get_name (directory));

  /* Fetch submenus */
  menus = xfce_menu_get_menus (menu);

  /* Print child menus */
  for (iter = menus; iter != NULL; iter = g_slist_next (iter)) 
    {
      XfceMenuDirectory *submenu_directory = xfce_menu_get_directory (XFCE_MENU (iter->data));

      /* Don't display hidden menus */
      if (G_LIKELY (submenu_directory == NULL || !xfce_menu_directory_get_no_display (submenu_directory)))
        print_menu (XFCE_MENU (iter->data), FALSE);
    }

  /* Free submenu list */
  g_slist_free (menus);

  /* Fetch menu items */
  items = xfce_menu_get_items (menu);

  /* Print menu items */
  for (iter = items; iter != NULL; iter = g_slist_next (iter)) 
    {
      XfceMenuItem      *item = iter->data;

      if (G_UNLIKELY (!xfce_menu_item_get_no_display (item)))
        g_printf ("%s/\t%s\t%s\n", name, xfce_menu_item_get_desktop_id (item), xfce_menu_item_get_filename (item));
    }

  /* Free menu item list */
  g_slist_free (items);
}



int
main (int    argc,
      char **argv)
{
  XfceMenu *menu;
  GError   *error = NULL;
#ifdef HAVE_STDLIB_H
  int       exit_code = EXIT_SUCCESS;
#else
  int       exit_code = 0;
#endif

  /* Initialize menu library */
  xfce_menu_init (NULL);

  /* Try to get the root menu */
  menu = xfce_menu_get_root (&error);

  if (G_LIKELY (menu != NULL)) 
    {
      /* Print menu contents according to the test suite criteria */
      print_menu (menu, TRUE);
    }
  else
    {
      g_error (error->message);
      g_error_free (error);
#ifdef HAVE_STDLIB_H
      exit_code = EXIT_FAILURE;
#else
      exit_code = -1;
#endif
    }

  /* Shut down the menu library */
  xfce_menu_shutdown ();

  return exit_code;
}