/* vi:set sw=2 sts=2 ts=2 et ai: */
/*-
 * Copyright (c) 2009 Jannis Pohlmann <jannis@expidus.org>.
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

#include <glib.h>
#include <glib-object.h>

#include <markon/markon-menu-item.h>
#include <markon/markon-menu-node.h>



/* Property identifiers */
enum
{
  PROP_0,
  PROP_NODE_TYPE,
};



static void markon_menu_node_finalize     (GObject             *object);
static void markon_menu_node_get_property (GObject             *object,
                                           guint                prop_id,
                                           GValue              *value,
                                           GParamSpec          *pspec);
static void markon_menu_node_set_property (GObject             *object,
                                           guint                prop_id,
                                           const GValue        *value,
                                           GParamSpec          *pspec);
static void markon_menu_node_free_data    (MarkonMenuNode      *node);



struct _MarkonMenuNodeClass
{
  GObjectClass __parent__;
};

union _MarkonMenuNodeData
{
  MarkonMenuLayoutMergeType layout_merge_type;
  struct
  {
    MarkonMenuMergeFileType type;
    gchar                  *filename;
  } merge_file;
  gchar                    *string;
};

struct _MarkonMenuNode
{
  GObject            __parent__;

  MarkonMenuNodeType node_type;
  MarkonMenuNodeData data;
};



GType
markon_menu_node_type_get_type (void)
{
  static GType      type = G_TYPE_INVALID;
  static GEnumValue values[] =
  {
    { MARKON_MENU_NODE_TYPE_INVALID, "MARKON_MENU_NODE_TYPE_INVALID", "Invalid" },
    { MARKON_MENU_NODE_TYPE_MENU, "MARKON_MENU_NODE_TYPE_MENU", "Menu" },
    { MARKON_MENU_NODE_TYPE_NAME, "MARKON_MENU_NODE_TYPE_NAME", "Name" },
    { MARKON_MENU_NODE_TYPE_DIRECTORY, "MARKON_MENU_NODE_TYPE_DIRECTORY", "Directory" },
    { MARKON_MENU_NODE_TYPE_DIRECTORY_DIR, "MARKON_MENU_NODE_TYPE_DIRECTORY_DIR", "DirectoryDir" },
    { MARKON_MENU_NODE_TYPE_DEFAULT_DIRECTORY_DIRS, "MARKON_MENU_NODE_TYPE_DEFAULT_DIRECTORY_DIRS", "DefaultDirectoryDirs" },
    { MARKON_MENU_NODE_TYPE_APP_DIR, "MARKON_MENU_NODE_TYPE_APP_DIR", "AppDir" },
    { MARKON_MENU_NODE_TYPE_DEFAULT_APP_DIRS, "MARKON_MENU_NODE_TYPE_DEFAULT_APP_DIRS", "DefaultAppDirs" },
    { MARKON_MENU_NODE_TYPE_ONLY_UNALLOCATED, "MARKON_MENU_NODE_TYPE_ONLY_UNALLOCATED", "OnlyUnallocated" },
    { MARKON_MENU_NODE_TYPE_NOT_ONLY_UNALLOCATED, "MARKON_MENU_NODE_TYPE_NOT_ONLY_UNALLOCATED", "NotOnlyUnallocated" },
    { MARKON_MENU_NODE_TYPE_DELETED, "MARKON_MENU_NODE_TYPE_DELETED", "Deleted" },
    { MARKON_MENU_NODE_TYPE_NOT_DELETED, "MARKON_MENU_NODE_TYPE_NOT_DELETED", "NotDeleted" },
    { MARKON_MENU_NODE_TYPE_INCLUDE, "MARKON_MENU_NODE_TYPE_INCLUDE", "Include" },
    { MARKON_MENU_NODE_TYPE_EXCLUDE, "MARKON_MENU_NODE_TYPE_EXCLUDE", "Exclude" },
    { MARKON_MENU_NODE_TYPE_ALL, "MARKON_MENU_NODE_TYPE_ALL", "All" },
    { MARKON_MENU_NODE_TYPE_FILENAME, "MARKON_MENU_NODE_TYPE_FILENAME", "Filename" },
    { MARKON_MENU_NODE_TYPE_CATEGORY, "MARKON_MENU_NODE_TYPE_CATEGORY", "Category" },
    { MARKON_MENU_NODE_TYPE_OR, "MARKON_MENU_NODE_TYPE_OR", "Or" },
    { MARKON_MENU_NODE_TYPE_AND, "MARKON_MENU_NODE_TYPE_AND", "And" },
    { MARKON_MENU_NODE_TYPE_NOT, "MARKON_MENU_NODE_TYPE_NOT", "Not" },
    { MARKON_MENU_NODE_TYPE_MOVE, "MARKON_MENU_NODE_TYPE_MOVE", "Move" },
    { MARKON_MENU_NODE_TYPE_OLD, "MARKON_MENU_NODE_TYPE_OLD", "Old" },
    { MARKON_MENU_NODE_TYPE_NEW, "MARKON_MENU_NODE_TYPE_NEW", "New" },
    { MARKON_MENU_NODE_TYPE_DEFAULT_LAYOUT, "MARKON_MENU_NODE_TYPE_DEFAULT_LAYOUT", "DefaultLayout" },
    { MARKON_MENU_NODE_TYPE_LAYOUT, "MARKON_MENU_NODE_TYPE_LAYOUT", "Layout" },
    { MARKON_MENU_NODE_TYPE_MENUNAME, "MARKON_MENU_NODE_TYPE_MENUNAME", "Menuname" },
    { MARKON_MENU_NODE_TYPE_SEPARATOR, "MARKON_MENU_NODE_TYPE_SEPARATOR", "Separator" },
    { MARKON_MENU_NODE_TYPE_MERGE, "MARKON_MENU_NODE_TYPE_MERGE", "Merge" },
    { MARKON_MENU_NODE_TYPE_MERGE_FILE, "MARKON_MENU_NODE_TYPE_MERGE_FILE", "MergeFile" },
    { MARKON_MENU_NODE_TYPE_MERGE_DIR, "MARKON_MENU_NODE_TYPE_MERGE_DIR", "MergeDir" },
    { MARKON_MENU_NODE_TYPE_DEFAULT_MERGE_DIRS, "MARKON_MENU_NODE_TYPE_DEFAULT_MERGE_DIRS", "MergeDirs" },
    { 0, NULL, NULL },
  };

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    type = g_enum_register_static ("MarkonMenuNodeType", values);

  return type;
}



G_DEFINE_TYPE (MarkonMenuNode, markon_menu_node, G_TYPE_OBJECT)



static void
markon_menu_node_class_init (MarkonMenuNodeClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = markon_menu_node_finalize;
  gobject_class->get_property = markon_menu_node_get_property;
  gobject_class->set_property = markon_menu_node_set_property;

  g_object_class_install_property (gobject_class,
                                   PROP_NODE_TYPE,
                                   g_param_spec_enum ("node-type",
                                                      "node-type",
                                                      "node-type",
                                                      markon_menu_node_type_get_type (),
                                                      MARKON_MENU_NODE_TYPE_MENU,
                                                      G_PARAM_READWRITE |
                                                      G_PARAM_STATIC_STRINGS));
}



static void
markon_menu_node_init (MarkonMenuNode *node)
{
}



static void
markon_menu_node_finalize (GObject *object)
{
  MarkonMenuNode *node = MARKON_MENU_NODE (object);

  markon_menu_node_free_data (node);

  (*G_OBJECT_CLASS (markon_menu_node_parent_class)->finalize) (object);
}



static void
markon_menu_node_get_property (GObject    *object,
                               guint       prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  MarkonMenuNode *node = MARKON_MENU_NODE (object);

  switch (prop_id)
    {
    case PROP_NODE_TYPE:
      g_value_set_enum (value, node->node_type);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
markon_menu_node_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
  MarkonMenuNode *node = MARKON_MENU_NODE (object);

  switch (prop_id)
    {
    case PROP_NODE_TYPE:
      node->node_type = g_value_get_enum (value);
      g_object_notify (G_OBJECT (node), "node-type");
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



MarkonMenuNode *
markon_menu_node_new (MarkonMenuNodeType node_type)
{
  return g_object_new (MARKON_TYPE_MENU_NODE, "node-type", node_type, NULL);
}



MarkonMenuNodeType markon_menu_node_get_node_type (MarkonMenuNode *node)
{
  g_return_val_if_fail (MARKON_IS_MENU_NODE (node), 0);
  return node->node_type;
}


/**
 * markon_menu_node_create:
 * @node_type: a #MarkonMenuNodeType
 * @first_value:
 * @...:
 *
 * Returns: (transfer full): a #MarkonMenuNode
 */
MarkonMenuNode *
markon_menu_node_create (MarkonMenuNodeType node_type,
                         gpointer           first_value,
                         ...)
{
  MarkonMenuNode *node;

  node = markon_menu_node_new (node_type);

  switch (node_type)
    {
    case MARKON_MENU_NODE_TYPE_NAME:
    case MARKON_MENU_NODE_TYPE_DIRECTORY:
    case MARKON_MENU_NODE_TYPE_DIRECTORY_DIR:
    case MARKON_MENU_NODE_TYPE_APP_DIR:
    case MARKON_MENU_NODE_TYPE_FILENAME:
    case MARKON_MENU_NODE_TYPE_CATEGORY:
    case MARKON_MENU_NODE_TYPE_OLD:
    case MARKON_MENU_NODE_TYPE_NEW:
    case MARKON_MENU_NODE_TYPE_MENUNAME:
    case MARKON_MENU_NODE_TYPE_MERGE_DIR:
      node->data.string = g_strdup (first_value);
      break;

    case MARKON_MENU_NODE_TYPE_MERGE:
      node->data.layout_merge_type = GPOINTER_TO_UINT (first_value);
      break;

    case MARKON_MENU_NODE_TYPE_MERGE_FILE:
      node->data.merge_file.type = GPOINTER_TO_UINT (first_value);
      node->data.merge_file.filename = NULL;
      break;

    default:
      break;
    }

  return node;
}


/**
 * markon_menu_node_copy:
 * @node: a #MarkonMenuNode
 * @data:
 *
 * Returns: (transfer full): a #MarkonMenuNode
 */
MarkonMenuNode *
markon_menu_node_copy (MarkonMenuNode *node,
                       gpointer        data)
{
  MarkonMenuNode *copy;

  if (node == NULL || !MARKON_IS_MENU_NODE (node))
    return NULL;

  copy = markon_menu_node_new (node->node_type);

  switch (copy->node_type)
    {
    case MARKON_MENU_NODE_TYPE_NAME:
    case MARKON_MENU_NODE_TYPE_DIRECTORY:
    case MARKON_MENU_NODE_TYPE_DIRECTORY_DIR:
    case MARKON_MENU_NODE_TYPE_APP_DIR:
    case MARKON_MENU_NODE_TYPE_FILENAME:
    case MARKON_MENU_NODE_TYPE_CATEGORY:
    case MARKON_MENU_NODE_TYPE_OLD:
    case MARKON_MENU_NODE_TYPE_NEW:
    case MARKON_MENU_NODE_TYPE_MENUNAME:
    case MARKON_MENU_NODE_TYPE_MERGE_DIR:
      copy->data.string = g_strdup (node->data.string);
      break;

    case MARKON_MENU_NODE_TYPE_MERGE:
      copy->data.layout_merge_type = node->data.layout_merge_type;
      break;

    case MARKON_MENU_NODE_TYPE_MERGE_FILE:
      copy->data.merge_file.type = node->data.merge_file.type;
      copy->data.merge_file.filename = g_strdup (node->data.merge_file.filename);
      break;

    default:
      break;
    }

  return copy;
}



static void
markon_menu_node_free_data (MarkonMenuNode *node)
{
  g_return_if_fail (MARKON_IS_MENU_NODE (node));

  switch (node->node_type)
    {
    case MARKON_MENU_NODE_TYPE_NAME:
    case MARKON_MENU_NODE_TYPE_DIRECTORY:
    case MARKON_MENU_NODE_TYPE_DIRECTORY_DIR:
    case MARKON_MENU_NODE_TYPE_APP_DIR:
    case MARKON_MENU_NODE_TYPE_FILENAME:
    case MARKON_MENU_NODE_TYPE_CATEGORY:
    case MARKON_MENU_NODE_TYPE_OLD:
    case MARKON_MENU_NODE_TYPE_NEW:
    case MARKON_MENU_NODE_TYPE_MENUNAME:
    case MARKON_MENU_NODE_TYPE_MERGE_DIR:
      g_free (node->data.string);
      break;

    case MARKON_MENU_NODE_TYPE_MERGE_FILE:
      g_free (node->data.merge_file.filename);
      break;

    default:
      break;
    }
}



const gchar *
markon_menu_node_get_string (MarkonMenuNode *node)
{
  g_return_val_if_fail (MARKON_IS_MENU_NODE (node), NULL);
  return node->data.string;
}



void
markon_menu_node_set_string (MarkonMenuNode *node,
                             const gchar    *value)
{
  g_return_if_fail (MARKON_IS_MENU_NODE (node));
  g_return_if_fail (value != NULL);

  g_free (node->data.string);
  node->data.string = g_strdup (value);
}



MarkonMenuMergeFileType
markon_menu_node_get_merge_file_type (MarkonMenuNode *node)
{
  g_return_val_if_fail (MARKON_IS_MENU_NODE (node), 0);
  g_return_val_if_fail (node->node_type == MARKON_MENU_NODE_TYPE_MERGE_FILE, 0);
  return node->data.merge_file.type;
}



void
markon_menu_node_set_merge_file_type (MarkonMenuNode         *node,
                                      MarkonMenuMergeFileType type)
{
  g_return_if_fail (MARKON_IS_MENU_NODE (node));
  g_return_if_fail (node->node_type == MARKON_MENU_NODE_TYPE_MERGE_FILE);
  node->data.merge_file.type = type;
}



const gchar *
markon_menu_node_get_merge_file_filename (MarkonMenuNode *node)
{
  g_return_val_if_fail (MARKON_IS_MENU_NODE (node), NULL);
  g_return_val_if_fail (node->node_type == MARKON_MENU_NODE_TYPE_MERGE_FILE, NULL);
  return node->data.merge_file.filename;
}



void
markon_menu_node_set_merge_file_filename (MarkonMenuNode *node,
                                          const gchar    *filename)
{
  g_return_if_fail (MARKON_IS_MENU_NODE (node));
  g_return_if_fail (filename != NULL);
  g_return_if_fail (node->node_type == MARKON_MENU_NODE_TYPE_MERGE_FILE);

  g_free (node->data.merge_file.filename);
  node->data.merge_file.filename = g_strdup (filename);
}



typedef struct
{
  MarkonMenuNodeType type;
  GNode             *self;
  gpointer           value;
} Pair;



static gboolean
collect_children (GNode *node,
                  Pair  *pair)
{
  if (node == pair->self)
    return FALSE;

  if (markon_menu_node_tree_get_node_type (node) == pair->type)
    pair->value = g_list_prepend (pair->value, node);

  return FALSE;
}


/**
 * markon_menu_node_tree_get_child_node: (skip)
 * @tree: #GNode instance
 * @type: type for the menu nodes
 * @reverse:
 *
 * Returns: a #GNode if @type is valid menu nodes type.
 */
GNode *
markon_menu_node_tree_get_child_node (GNode             *tree,
                                      MarkonMenuNodeType type,
                                      gboolean           reverse)
{
  GNode *child = NULL;

  if (reverse)
    {
      for (child = g_node_last_child (tree); 
           child != NULL;
           child = g_node_prev_sibling (child))
        {
          if (markon_menu_node_tree_get_node_type (child) == type)
            return child;
        }
    }
  else
    {
      for (child = g_node_first_child (tree);
           child != NULL;
           child = g_node_next_sibling (child))
        {
          if (markon_menu_node_tree_get_node_type (child) == type)
            return child;
        }
    }

  return child;
}


/**
 * markon_menu_node_tree_get_child_nodes:
 * @tree: a GNode
 * @type: type for the menu nodes
 * @reverse:
 *
 * Returns: (element-type GNode) (transfer full): list of #GNode
 */
GList *
markon_menu_node_tree_get_child_nodes (GNode             *tree,
                                       MarkonMenuNodeType type,
                                       gboolean           reverse)
{
  Pair pair;

  pair.type = type;
  pair.value = NULL;
  pair.self = tree;

  g_node_traverse (tree, G_IN_ORDER, G_TRAVERSE_ALL, 2,
                   (GNodeTraverseFunc) collect_children, &pair);

  /* Return the list as if we appended */
  if (!reverse && pair.value != NULL)
    pair.value = g_list_reverse (pair.value);

  return (GList *) pair.value;
}



static gboolean
collect_strings (GNode *node,
                 Pair  *pair)
{
  gpointer string;

  if (node == pair->self)
    return FALSE;

  if (markon_menu_node_tree_get_node_type (node) == pair->type)
    {
      string = (gpointer) markon_menu_node_tree_get_string (node);
      pair->value = g_list_prepend (pair->value, string);
    }

  return FALSE;
}


/**
 * markon_menu_node_tree_get_string_children:
 * @tree: a #GNode instance
 * @type: type for the menu nodes
 * @reverse:
 *
 * Returns: (element-type GNode) (transfer full): list of #GNode
 */
GList *
markon_menu_node_tree_get_string_children (GNode             *tree,
                                           MarkonMenuNodeType type,
                                           gboolean           reverse)
{
  Pair pair;

  pair.type = type;
  pair.value = NULL;
  pair.self = tree;

  g_node_traverse (tree, G_IN_ORDER, G_TRAVERSE_ALL, 2,
                   (GNodeTraverseFunc) collect_strings, &pair);

  /* Return the list as if we appended */
  if (!reverse && pair.value != NULL)
    pair.value = g_list_reverse (pair.value);

  return (GList *) pair.value;
}



static gboolean
collect_boolean (GNode *node,
                 Pair  *pair)
{
  if (node == pair->self)
    return FALSE;

  if (markon_menu_node_tree_get_node_type (node) == pair->type)
    {
      pair->value = GUINT_TO_POINTER (1);
      return TRUE;
    }

  return FALSE;
}



gboolean
markon_menu_node_tree_get_boolean_child (GNode             *tree,
                                         MarkonMenuNodeType type)
{
  Pair pair;

  pair.value = GUINT_TO_POINTER (0);
  pair.self = tree;
  pair.type = type;

  g_node_traverse (tree, G_IN_ORDER, G_TRAVERSE_ALL, 2,
                   (GNodeTraverseFunc) collect_boolean, &pair);

  return !!GPOINTER_TO_UINT (pair.value);
}



static gboolean
collect_string (GNode *node,
                Pair  *pair)
{
  const gchar **string = pair->value;

  if (node == pair->self)
    return FALSE;

  if (markon_menu_node_tree_get_node_type (node) == pair->type)
    {
      *string = markon_menu_node_tree_get_string (node);
      return TRUE;
    }

  return FALSE;
}



const gchar *
markon_menu_node_tree_get_string_child (GNode             *tree,
                                        MarkonMenuNodeType type)
{
  Pair         pair;
  const gchar *string = NULL;

  pair.type = type;
  pair.value = &string;
  pair.self = tree;

  g_node_traverse (tree, G_IN_ORDER, G_TRAVERSE_ALL, 2,
                   (GNodeTraverseFunc) collect_string, &pair);

  return string;
}



gboolean
markon_menu_node_tree_rule_matches (GNode          *node,
                                    MarkonMenuItem *item)
{
  GNode   *child;
  gboolean matches = FALSE;
  gboolean child_matches = FALSE;

  switch (markon_menu_node_tree_get_node_type (node))
    {
    case MARKON_MENU_NODE_TYPE_CATEGORY:
      matches = markon_menu_item_has_category (item, markon_menu_node_tree_get_string (node));
      break;

    case MARKON_MENU_NODE_TYPE_INCLUDE:
    case MARKON_MENU_NODE_TYPE_EXCLUDE:
    case MARKON_MENU_NODE_TYPE_OR:
      for (child = g_node_first_child (node); child != NULL; child = g_node_next_sibling (child))
        matches = matches || markon_menu_node_tree_rule_matches (child, item);
      break;

    case MARKON_MENU_NODE_TYPE_FILENAME:
      matches = g_str_equal (markon_menu_node_tree_get_string (node),
                             markon_menu_item_get_desktop_id (item));
      break;

    case MARKON_MENU_NODE_TYPE_AND:
      matches = TRUE;
      for (child = g_node_first_child (node); child != NULL; child = g_node_next_sibling (child))
        matches = matches && markon_menu_node_tree_rule_matches (child, item);
      break;

    case MARKON_MENU_NODE_TYPE_NOT:
      for (child = g_node_first_child (node); child != NULL; child = g_node_next_sibling (child))
        child_matches = child_matches || markon_menu_node_tree_rule_matches (child, item);
      matches = !child_matches;
      break;

    case MARKON_MENU_NODE_TYPE_ALL:
      matches = TRUE;
      break;

    default:
      break;
    }

  return matches;
}






MarkonMenuNodeType
markon_menu_node_tree_get_node_type (GNode *tree)
{
  if (tree == NULL)
    return MARKON_MENU_NODE_TYPE_INVALID;

  if (tree->data == NULL)
    return MARKON_MENU_NODE_TYPE_MENU;

  return markon_menu_node_get_node_type (tree->data);
}



const gchar *
markon_menu_node_tree_get_string (GNode *tree)
{
  if (tree == NULL || tree->data == NULL)
    return NULL;
  else
    return markon_menu_node_get_string (tree->data);
}



void
markon_menu_node_tree_set_string (GNode       *tree,
                                  const gchar *value)
{
  MarkonMenuNodeType type;

  type = markon_menu_node_tree_get_node_type (tree);

  g_return_if_fail (type == MARKON_MENU_NODE_TYPE_NAME ||
                    type == MARKON_MENU_NODE_TYPE_DIRECTORY ||
                    type == MARKON_MENU_NODE_TYPE_DIRECTORY_DIR ||
                    type == MARKON_MENU_NODE_TYPE_APP_DIR ||
                    type == MARKON_MENU_NODE_TYPE_FILENAME ||
                    type == MARKON_MENU_NODE_TYPE_CATEGORY ||
                    type == MARKON_MENU_NODE_TYPE_OLD ||
                    type == MARKON_MENU_NODE_TYPE_NEW ||
                    type == MARKON_MENU_NODE_TYPE_MENUNAME ||
                    type == MARKON_MENU_NODE_TYPE_MERGE_DIR);

  markon_menu_node_set_string (tree->data, value);
}


MarkonMenuLayoutMergeType
markon_menu_node_tree_get_layout_merge_type (GNode *tree)
{
  g_return_val_if_fail (markon_menu_node_tree_get_node_type (tree) == MARKON_MENU_NODE_TYPE_MERGE, 0);
  return ((MarkonMenuNode *)tree->data)->data.layout_merge_type;
}



MarkonMenuMergeFileType
markon_menu_node_tree_get_merge_file_type (GNode *tree)
{
  g_return_val_if_fail (markon_menu_node_tree_get_node_type (tree) == MARKON_MENU_NODE_TYPE_MERGE_FILE, 0);
  return markon_menu_node_get_merge_file_type (tree->data);
}



const gchar *
markon_menu_node_tree_get_merge_file_filename (GNode *tree)
{
  g_return_val_if_fail (markon_menu_node_tree_get_node_type (tree) == MARKON_MENU_NODE_TYPE_MERGE_FILE, NULL);
  return markon_menu_node_get_merge_file_filename (tree->data);
}



void
markon_menu_node_tree_set_merge_file_filename (GNode       *tree,
                                                  const gchar *filename)
{
  g_return_if_fail (markon_menu_node_tree_get_node_type (tree) == MARKON_MENU_NODE_TYPE_MERGE_FILE);
  markon_menu_node_set_merge_file_filename (tree->data, filename);
}



gint
markon_menu_node_tree_compare (GNode *tree,
                               GNode *other_tree)
{
  MarkonMenuNode *node;
  MarkonMenuNode *other_node;

  if (tree == NULL || other_tree == NULL)
    return 0;

  node = tree->data;
  other_node = other_tree->data;

  if (node->node_type != other_node->node_type)
    return 0;

  switch (node->node_type)
    {
    case MARKON_MENU_NODE_TYPE_NAME:
    case MARKON_MENU_NODE_TYPE_DIRECTORY:
    case MARKON_MENU_NODE_TYPE_DIRECTORY_DIR:
    case MARKON_MENU_NODE_TYPE_APP_DIR:
    case MARKON_MENU_NODE_TYPE_FILENAME:
    case MARKON_MENU_NODE_TYPE_CATEGORY:
    case MARKON_MENU_NODE_TYPE_OLD:
    case MARKON_MENU_NODE_TYPE_NEW:
    case MARKON_MENU_NODE_TYPE_MENUNAME:
    case MARKON_MENU_NODE_TYPE_MERGE_DIR:
      return g_strcmp0 (node->data.string, other_node->data.string);
      break;

    case MARKON_MENU_NODE_TYPE_MERGE_FILE:
      return g_strcmp0 (node->data.merge_file.filename,
                        other_node->data.merge_file.filename);
      break;

    default:
      return 0;
      break;
    }

  return 0;
}


/**
 * markon_menu_node_tree_copy: (skip)
 * @tree: a #GNode
 *
 * Recursively copies a #GNode.
 */
GNode *
markon_menu_node_tree_copy (GNode *tree)
{
  return g_node_copy_deep (tree, (GCopyFunc) markon_menu_node_copy, NULL);
}



static gboolean
free_children (GNode   *tree,
               gpointer data)
{
  markon_menu_node_tree_free_data (tree);
  return FALSE;
}



void
markon_menu_node_tree_free (GNode *tree)
{
  if (tree != NULL)
    {
      g_node_traverse (tree, G_IN_ORDER, G_TRAVERSE_ALL, -1,
                       (GNodeTraverseFunc) free_children, NULL);

      g_node_destroy (tree);
    }
}



void
markon_menu_node_tree_free_data (GNode *tree)
{
  if (tree != NULL && tree->data != NULL)
    g_object_unref (tree->data);
}

