/* bst/gsl_bst_rb.h
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

#ifndef __GSL_BST_RB_H__
#define __GSL_BST_RB_H__

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

#ifndef GSL_BST_RB_MAX_HEIGHT
#define GSL_BST_RB_MAX_HEIGHT 48
#endif

/* red-black node */
struct gsl_bst_rb_node
{
  struct gsl_bst_rb_node *rb_link[2]; /* subtrees */
  void *rb_data;                      /* pointer to data */
  unsigned char rb_color;             /* color */
};

/* red-black tree data structure */
typedef struct
{
  struct gsl_bst_rb_node *rb_root;   /* tree's root */
  gsl_bst_cmp_function *rb_compare;  /* comparison function */
  void *rb_param;                    /* extra argument to |rb_compare| */
  const gsl_bst_allocator *rb_alloc; /* memory allocator */
  size_t rb_count;                   /* number of items in tree */
  unsigned long rb_generation;       /* generation number */
} gsl_bst_rb_table;

/* red-black traverser structure */
typedef struct
{
  const gsl_bst_rb_table *rb_table;  /* tree being traversed */
  struct gsl_bst_rb_node *rb_node;   /* current node in tree */
  struct gsl_bst_rb_node *rb_stack[GSL_BST_RB_MAX_HEIGHT];
                                     /* all the nodes above |rb_node| */
  size_t rb_height;                  /* number of nodes in |rb_parent| */
  unsigned long rb_generation;       /* generation number */
} gsl_bst_rb_traverser;

__END_DECLS

#endif /* __GSL_BST_RB_H__ */
