/* $Id$ */
/* vi:set expandtab sw=2 sts=2: */
/*-
 * Copyright (c) 2008 Jannis Pohlmann <jannis@xfce.org>
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

#if !defined (LIBXFCE4MENU_INSIDE_LIBXFCE4MENU_H) && !defined (LIBXFCE4MENU_COMPILATION)
#error "Only <libxfce4menu/libxfce4menu.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __LIBXFCE4MENU_CONFIG_H__
#define __LIBXFCE4MENU_CONFIG_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define LIBXFCE4MENU_MAJOR_VERSION 4
#define LIBXFCE4MENU_MINOR_VERSION 5
#define LIBXFCE4MENU_MICRO_VERSION 99

#define LIBXFCE4MENU_CHECK_VERSION(major,minor,micro) \
  (LIBXFCE4MENU_MAJOR_VERSION > (major) \
   || (LIBXFCE4MENU_MAJOR_VERSION == (major) \
       && LIBXFCE4MENU_MINOR_VERSION > (minor)) \
   || (LIBXFCE4MENU_MAJOR_VERSION == (major) \
       && LIBXFCE4MENU_MINOR_VERSION == (minor) \
       && LIBXFCE4MENU_MICRO_VERSION >= (micro)))

extern const guint libxfce4menu_major_version;
extern const guint libxfce4menu_minor_version;
extern const guint libxfce4menu_micro_version;

const gchar *libxfce4menu_check_version (guint required_major,
                                         guint required_minor,
                                         guint required_micro);

G_END_DECLS

#endif /* !__LIBXFCE4MENU_CONFIG_H__ */
