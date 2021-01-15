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

#ifndef __MARKON_MENU_NODE_H__
#define __MARKON_MENU_NODE_H__

#include <markon/markon.h>

G_BEGIN_DECLS

/* Types for the menu nodes */
typedef enum
{
  MARKON_MENU_NODE_TYPE_INVALID,
  MARKON_MENU_NODE_TYPE_MENU,
  MARKON_MENU_NODE_TYPE_NAME,
  MARKON_MENU_NODE_TYPE_DIRECTORY,
  MARKON_MENU_NODE_TYPE_DIRECTORY_DIR,
  MARKON_MENU_NODE_TYPE_DEFAULT_DIRECTORY_DIRS,
  MARKON_MENU_NODE_TYPE_APP_DIR,
  MARKON_MENU_NODE_TYPE_DEFAULT_APP_DIRS,
  MARKON_MENU_NODE_TYPE_ONLY_UNALLOCATED,
  MARKON_MENU_NODE_TYPE_NOT_ONLY_UNALLOCATED,
  MARKON_MENU_NODE_TYPE_DELETED,
  MARKON_MENU_NODE_TYPE_NOT_DELETED,
  MARKON_MENU_NODE_TYPE_INCLUDE,
  MARKON_MENU_NODE_TYPE_EXCLUDE,
  MARKON_MENU_NODE_TYPE_ALL,
  MARKON_MENU_NODE_TYPE_FILENAME,
  MARKON_MENU_NODE_TYPE_CATEGORY,
  MARKON_MENU_NODE_TYPE_OR,
  MARKON_MENU_NODE_TYPE_AND,
  MARKON_MENU_NODE_TYPE_NOT,
  MARKON_MENU_NODE_TYPE_MOVE,
  MARKON_MENU_NODE_TYPE_OLD,
  MARKON_MENU_NODE_TYPE_NEW,
  MARKON_MENU_NODE_TYPE_DEFAULT_LAYOUT,
  MARKON_MENU_NODE_TYPE_LAYOUT,
  MARKON_MENU_NODE_TYPE_MENUNAME,
  MARKON_MENU_NODE_TYPE_SEPARATOR,
  MARKON_MENU_NODE_TYPE_MERGE,
  MARKON_MENU_NODE_TYPE_MERGE_FILE,
  MARKON_MENU_NODE_TYPE_MERGE_DIR,
  MARKON_MENU_NODE_TYPE_DEFAULT_MERGE_DIRS,
} MarkonMenuNodeType;



typedef enum
{
  MARKON_MENU_LAYOUT_MERGE_MENUS,
  MARKON_MENU_LAYOUT_MERGE_FILES,
  MARKON_MENU_LAYOUT_MERGE_ALL,
} MarkonMenuLayoutMergeType;

typedef enum
{
  MARKON_MENU_MERGE_FILE_PATH,
  MARKON_MENU_MERGE_FILE_PARENT,
} MarkonMenuMergeFileType;



typedef union  _MarkonMenuNodeData  MarkonMenuNodeData;
typedef struct _MarkonMenuNodeClass MarkonMenuNodeClass;
typedef struct _MarkonMenuNode      MarkonMenuNode;

#define MARKON_TYPE_MENU_NODE            (markon_menu_node_get_type ())
#define MARKON_MENU_NODE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), MARKON_TYPE_MENU_NODE, MarkonMenuNode))
#define MARKON_MENU_NODE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), MARKON_TYPE_MENU_NODE, MarkonMenuNodeClass))
#define MARKON_IS_MENU_NODE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MARKON_TYPE_MENU_NODE))
#define MARKON_IS_MENU_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), MARKON_TYPE_MENU_NODE)
#define MARKON_MENU_NODE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), MARKON_TYPE_MENU_NODE, MarkonMenuNodeClass))

GType                     markon_menu_node_type_get_type                (void);
GType                     markon_menu_node_get_type                     (void) G_GNUC_CONST;

MarkonMenuNode           *markon_menu_node_new                          (MarkonMenuNodeType      node_type) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
MarkonMenuNodeType        markon_menu_node_get_node_type                (MarkonMenuNode         *node);
MarkonMenuNode           *markon_menu_node_create                       (MarkonMenuNodeType      node_type,
                                                                         gpointer                first_value,
                                                                         ...) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
MarkonMenuNode           *markon_menu_node_copy                         (MarkonMenuNode         *node,
                                                                         gpointer                data) G_GNUC_MALLOC G_GNUC_WARN_UNUSED_RESULT;
const gchar              *markon_menu_node_get_string                   (MarkonMenuNode         *node);
void                      markon_menu_node_set_string                   (MarkonMenuNode         *node,
                                                                         const gchar            *value);
MarkonMenuMergeFileType   markon_menu_node_get_merge_file_type          (MarkonMenuNode         *node);
void                      markon_menu_node_set_merge_file_type          (MarkonMenuNode         *node,
                                                                         MarkonMenuMergeFileType type);
const gchar              *markon_menu_node_get_merge_file_filename      (MarkonMenuNode         *node);
void                      markon_menu_node_set_merge_file_filename      (MarkonMenuNode         *node,
                                                                         const gchar            *filename);

GNode                    *markon_menu_node_tree_get_child_node          (GNode                  *tree,
                                                                         MarkonMenuNodeType      type,
                                                                         gboolean                reverse);
GList                    *markon_menu_node_tree_get_child_nodes         (GNode                  *tree,
                                                                         MarkonMenuNodeType      type,
                                                                         gboolean                reverse);
GList                    *markon_menu_node_tree_get_string_children     (GNode                  *tree,
                                                                         MarkonMenuNodeType      type,
                                                                         gboolean                reverse);
gboolean                  markon_menu_node_tree_get_boolean_child       (GNode                  *tree,
                                                                         MarkonMenuNodeType      type);
const gchar              *markon_menu_node_tree_get_string_child        (GNode                  *tree,
                                                                         MarkonMenuNodeType      type);
gboolean                  markon_menu_node_tree_rule_matches            (GNode                  *tree,
                                                                         MarkonMenuItem         *item);
MarkonMenuNodeType        markon_menu_node_tree_get_node_type           (GNode                  *tree);
const gchar              *markon_menu_node_tree_get_string              (GNode                  *tree);
void                      markon_menu_node_tree_set_string              (GNode                  *tree,
                                                                         const gchar            *value);
MarkonMenuLayoutMergeType markon_menu_node_tree_get_layout_merge_type   (GNode                  *tree);
MarkonMenuMergeFileType   markon_menu_node_tree_get_merge_file_type     (GNode                  *tree);
const gchar              *markon_menu_node_tree_get_merge_file_filename (GNode                  *tree);
void                      markon_menu_node_tree_set_merge_file_filename (GNode                  *tree,
                                                                         const gchar            *filename);
gint                      markon_menu_node_tree_compare                 (GNode                  *tree,
                                                                         GNode                  *other_tree);
GNode                    *markon_menu_node_tree_copy                    (GNode                  *tree);
void                      markon_menu_node_tree_free                    (GNode                  *tree);
void                      markon_menu_node_tree_free_data               (GNode                  *tree);


G_END_DECLS

#endif /* !__MARKON_MENU_NODE_H__ */
