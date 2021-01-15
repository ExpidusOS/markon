/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2007-2009 Jannis Pohlmann <jannis@expidus.org>
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

#ifndef __MARKON_MENU_DIRECTORY_H__
#define __MARKON_MENU_DIRECTORY_H__

#include <gio/gio.h>

G_BEGIN_DECLS

#define MARKON_TYPE_MENU_DIRECTORY            (markon_menu_directory_get_type ())
#define MARKON_MENU_DIRECTORY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MARKON_TYPE_MENU_DIRECTORY, MarkonMenuDirectory))
#define MARKON_MENU_DIRECTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MARKON_TYPE_MENU_DIRECTORY, MarkonMenuDirectoryClass))
#define MARKON_IS_MENU_DIRECTORY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MARKON_TYPE_MENU_DIRECTORY))
#define MARKON_IS_MENU_DIRECTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MARKON_TYPE_MENU_DIRECTORY))
#define MARKON_MENU_DIRECTORY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MARKON_TYPE_MENU_DIRECTORY, MarkonMenuDirectoryClass))

typedef struct _MarkonMenuDirectoryPrivate MarkonMenuDirectoryPrivate;
typedef struct _MarkonMenuDirectoryClass   MarkonMenuDirectoryClass;
typedef struct _MarkonMenuDirectory        MarkonMenuDirectory;

struct _MarkonMenuDirectoryClass
{
  GObjectClass __parent__;
};

struct _MarkonMenuDirectory
{
  GObject __parent__;

  /* < private > */
  MarkonMenuDirectoryPrivate *priv;
};



GType                markon_menu_directory_get_type                (void) G_GNUC_CONST;

MarkonMenuDirectory *markon_menu_directory_new                     (GFile               *file) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;

GFile               *markon_menu_directory_get_file                (MarkonMenuDirectory *directory);
const gchar         *markon_menu_directory_get_name                (MarkonMenuDirectory *directory);
void                 markon_menu_directory_set_name                (MarkonMenuDirectory *directory,
                                                                    const gchar         *name);
const gchar         *markon_menu_directory_get_comment             (MarkonMenuDirectory *directory);
void                 markon_menu_directory_set_comment             (MarkonMenuDirectory *directory,
                                                                    const gchar         *comment);
const gchar         *markon_menu_directory_get_icon_name           (MarkonMenuDirectory *directory);
void                 markon_menu_directory_set_icon_name           (MarkonMenuDirectory *directory,
                                                                    const gchar         *icon_name);
gboolean             markon_menu_directory_get_no_display          (MarkonMenuDirectory *directory);
void                 markon_menu_directory_set_no_display          (MarkonMenuDirectory *directory,
                                                                    gboolean             no_display);
gboolean             markon_menu_directory_get_hidden              (MarkonMenuDirectory *directory);
gboolean             markon_menu_directory_get_show_in_environment (MarkonMenuDirectory *directory);
gboolean             markon_menu_directory_get_visible             (MarkonMenuDirectory *directory);
gboolean             markon_menu_directory_equal                   (MarkonMenuDirectory *directory,
                                                                    MarkonMenuDirectory *other);

G_END_DECLS

#endif /* !__MARKON_MENU_DIRECTORY_H__ */
