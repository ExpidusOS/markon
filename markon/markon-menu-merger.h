/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2009-2010 Jannis Pohlmann <jannis@expidus.org>
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

#if !defined (MARKON_INSIDE_MARKON_H) && !defined (MARKON_COMPILATION)
#error "Only <markon/markon.h> can be included directly. This file may disappear or change contents."
#endif

#ifndef __MARKON_MENU_MERGER_H__
#define __MARKON_MENU_MERGER_H__

#include <markon/markon.h>
#include <markon/markon-menu-tree-provider.h>

G_BEGIN_DECLS

#define MARKON_TYPE_MENU_MERGER            (markon_menu_merger_get_type ())
#define MARKON_MENU_MERGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MARKON_TYPE_MENU_MERGER, MarkonMenuMerger))
#define MARKON_MENU_MERGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MARKON_TYPE_MENU_MERGER, MarkonMenuMergerClass))
#define MARKON_IS_MENU_MERGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MARKON_TYPE_MENU_MERGER))
#define MARKON_IS_MENU_MERGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MARKON_TYPE_MENU_MERGER)
#define MARKON_MENU_MERGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MARKON_TYPE_MENU_MERGER, MarkonMenuMergerClass))

typedef struct _MarkonMenuMergerPrivate MarkonMenuMergerPrivate;
typedef struct _MarkonMenuMergerClass   MarkonMenuMergerClass;
typedef struct _MarkonMenuMerger        MarkonMenuMerger;

GType             markon_menu_merger_get_type (void) G_GNUC_CONST;

MarkonMenuMerger *markon_menu_merger_new      (MarkonMenuTreeProvider *provider) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
gboolean          markon_menu_merger_run      (MarkonMenuMerger       *merger,
                                               GList                 **merge_files,
                                               GList                 **merge_dirs,
                                               GCancellable           *cancellable,
                                               GError                **error);



struct _MarkonMenuMergerClass
{
  GObjectClass __parent__;
};

struct _MarkonMenuMerger
{
  GObject                  __parent__;

  MarkonMenuMergerPrivate *priv;
};

G_END_DECLS

#endif /* !__MARKON_MENU_MERGER_H__ */
