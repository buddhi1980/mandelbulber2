/* bst/gsl_bst.h
 * 
 * Copyright (C) 2018, 2019 Patrick Alken
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

#ifndef __GSL_BST_H__
#define __GSL_BST_H__

#include <stdlib.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_bst_avl.h>
#include <gsl/gsl_bst_rb.h>
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

/* type of binary search tree */
typedef struct
{
  const char *name;
  const size_t node_size;
  int    (*init)(const gsl_bst_allocator * allocator,
                 gsl_bst_cmp_function * compare, void * params, void * vtable);
  size_t (*nodes) (const void * vtable);
  void * (*insert) (void * item, void * vtable);
  void * (*find) (const void *item, const void * vtable);
  void * (*remove) (const void *item, void * vtable);
  int    (*empty) (void * vtable);
  int    (*trav_init) (void * vtrav, const void * vtable);
  void * (*trav_first) (void * vtrav, const void * vtable);
  void * (*trav_last) (void * vtrav, const void * vtable);
  void * (*trav_find) (const void * item, void * vtrav, const void * vtable);
  void * (*trav_insert) (void * item, void * vtrav, void * vtable);
  void * (*trav_copy) (void * vtrav, const void * vsrc);
  void * (*trav_next) (void * vtrav);
  void * (*trav_prev) (void * vtrav);
  void * (*trav_cur) (const void * vtrav);
  void * (*trav_replace) (void * vtrav, void * new_item);
} gsl_bst_type;

typedef struct
{
  const gsl_bst_type * type;    /* binary search tree type */
  union
    {
      gsl_bst_avl_table avl_table;
      gsl_bst_rb_table rb_table;
    } table;
} gsl_bst_workspace;

typedef struct
{
  const gsl_bst_type * type;    /* binary search tree type */
  union
    {
      gsl_bst_avl_traverser avl_trav;
      gsl_bst_rb_traverser rb_trav;
    } trav_data;
} gsl_bst_trav;

/* tree types */
GSL_VAR const gsl_bst_type * gsl_bst_avl;
GSL_VAR const gsl_bst_type * gsl_bst_rb;

/*
 * Prototypes
 */

gsl_bst_workspace * gsl_bst_alloc(const gsl_bst_type * T, const gsl_bst_allocator * allocator,
                                  gsl_bst_cmp_function * compare, void * params);
void gsl_bst_free(gsl_bst_workspace * w);
int gsl_bst_empty(gsl_bst_workspace * w);
void * gsl_bst_insert(void * item, gsl_bst_workspace * w);
void * gsl_bst_find(const void * item, const gsl_bst_workspace * w);
void * gsl_bst_remove(const void * item, gsl_bst_workspace * w);
size_t gsl_bst_nodes(const gsl_bst_workspace * w);
size_t gsl_bst_node_size(const gsl_bst_workspace * w);
const char * gsl_bst_name(const gsl_bst_workspace * w);

int gsl_bst_trav_init(gsl_bst_trav * trav, const gsl_bst_workspace * w);
void * gsl_bst_trav_first(gsl_bst_trav * trav, const gsl_bst_workspace * w);
void * gsl_bst_trav_last (gsl_bst_trav * trav, const gsl_bst_workspace * w);
void * gsl_bst_trav_find (const void * item, gsl_bst_trav * trav, const gsl_bst_workspace * w);
void * gsl_bst_trav_insert (void * item, gsl_bst_trav * trav, gsl_bst_workspace * w);
void * gsl_bst_trav_copy(gsl_bst_trav * dest, const gsl_bst_trav * src);
void * gsl_bst_trav_next(gsl_bst_trav * trav);
void * gsl_bst_trav_prev(gsl_bst_trav * trav);
void * gsl_bst_trav_cur(const gsl_bst_trav * trav);
void * gsl_bst_trav_replace (gsl_bst_trav * trav, void * new_item);

__END_DECLS

#endif /* __GSL_BST_H__ */
