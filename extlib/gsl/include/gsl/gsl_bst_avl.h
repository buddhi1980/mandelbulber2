/* bst/gsl_bst_avl.h
 * 
 * Copyright (C) 2018 Patrick Alken
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __GSL_BST_AVL_H__
#define __GSL_BST_AVL_H__

#include <stdlib.h>
#include <gsl/gsl_bst_types.h>

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
#endif

__BEGIN_DECLS

#ifndef GSL_BST_AVL_MAX_HEIGHT
#define GSL_BST_AVL_MAX_HEIGHT 32
#endif

/* AVL node */
struct gsl_bst_avl_node
{
  struct gsl_bst_avl_node *avl_link[2]; /* subtrees */
  void *avl_data;                       /* pointer to data */
  signed char avl_balance;              /* balance factor */
};

/* tree data structure */
typedef struct
{
  struct gsl_bst_avl_node *avl_root;          /* tree's root */
  gsl_bst_cmp_function *avl_compare;          /* comparison function */
  void *avl_param;                            /* extra argument to |avl_compare| */
  const gsl_bst_allocator *avl_alloc;         /* memory allocator */
  size_t avl_count;                           /* number of items in tree */
  unsigned long avl_generation;               /* generation number */
} gsl_bst_avl_table;

/* AVL traverser structure */
typedef struct
{
  const gsl_bst_avl_table *avl_table;                         /* tree being traversed */
  struct gsl_bst_avl_node *avl_node;                          /* current node in tree */
  struct gsl_bst_avl_node *avl_stack[GSL_BST_AVL_MAX_HEIGHT]; /* all the nodes above |avl_node| */
  size_t avl_height;                                          /* number of nodes in |avl_parent| */
  unsigned long avl_generation;                               /* generation number */
} gsl_bst_avl_traverser;

__END_DECLS

#endif /* __GSL_BST_AVL_H__ */
