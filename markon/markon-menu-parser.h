/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2009 Jannis Pohlmann <jannis@expidus.org>
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

#ifndef __MARKON_MENU_PARSER_H__
#define __MARKON_MENU_PARSER_H__

#include <gio/gio.h>

G_BEGIN_DECLS

#define MARKON_TYPE_MENU_PARSER            (markon_menu_parser_get_type ())
#define MARKON_MENU_PARSER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MARKON_TYPE_MENU_PARSER, MarkonMenuParser))
#define MARKON_MENU_PARSER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MARKON_TYPE_MENU_PARSER, MarkonMenuParserClass))
#define MARKON_IS_MENU_PARSER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MARKON_TYPE_MENU_PARSER))
#define MARKON_IS_MENU_PARSER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MARKON_TYPE_MENU_PARSER)
#define MARKON_MENU_PARSER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MARKON_TYPE_MENU_PARSER, MarkonMenuParserClass))

typedef struct _MarkonMenuParserPrivate MarkonMenuParserPrivate;
typedef struct _MarkonMenuParserClass   MarkonMenuParserClass;
typedef struct _MarkonMenuParser        MarkonMenuParser;

GType             markon_menu_parser_get_type (void) G_GNUC_CONST;

MarkonMenuParser *markon_menu_parser_new      (GFile              *file) G_GNUC_MALLOC;
gboolean          markon_menu_parser_run      (MarkonMenuParser *parser,
                                               GCancellable     *cancellable,
                                               GError          **error);



struct _MarkonMenuParserClass
{
  GObjectClass __parent__;
};

struct _MarkonMenuParser
{
  GObject                  __parent__;

  MarkonMenuParserPrivate *priv;
};

G_END_DECLS

#endif /* !__MARKON_MENU_PARSER_H__ */
