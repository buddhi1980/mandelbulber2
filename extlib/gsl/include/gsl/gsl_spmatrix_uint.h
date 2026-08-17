/* spmatrix/gsl_spmatrix_uint.h
 * 
 * Copyright (C) 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019, 2020 Patrick Alken
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

#ifndef __GSL_SPMATRIX_UINT_H__
#define __GSL_SPMATRIX_UINT_H__

#include <stdlib.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_bst.h>
#include <gsl/gsl_vector_uint.h>
#include <gsl/gsl_matrix_uint.h>

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

/*
 * COO format:
 *
 * If data[n] = A_{ij}, then:
 *   i = A->i[n]
 *   j = A->p[n]
 *
 * Compressed column format (CSC):
 *
 * If data[n] = A_{ij}, then:
 *   i = A->i[n]
 *   A->p[j] <= n < A->p[j+1]
 * so that column j is stored in
 * [ data[p[j]], data[p[j] + 1], ..., data[p[j+1] - 1] ]
 *
 * Compressed row format (CSR):
 *
 * If data[n] = A_{ij}, then:
 *   j = A->i[n]
 *   A->p[i] <= n < A->p[i+1]
 * so that row i is stored in
 * [ data[p[i]], data[p[i] + 1], ..., data[p[i+1] - 1] ]
 */

typedef struct
{
  size_t size1;               /* number of rows */
  size_t size2;               /* number of columns */

  /* i (size nzmax) contains:
   *
   * COO/CSC: row indices
   * CSR: column indices
   */
  int *i;

  unsigned int *data;               /* matrix elements of size nzmax */

  /*
   * COO: p[n] = column number of element data[n]
   * CSC: p[j] = index in data of first non-zero element in column j
   * CSR: p[i] = index in data of first non-zero element in row i
   */
  int *p;

  size_t nzmax;              /* maximum number of matrix elements */
  size_t nz;                 /* number of non-zero values in matrix */

  gsl_bst_workspace *tree;   /* binary tree structure */
  gsl_spmatrix_pool *pool;   /* memory pool for binary tree nodes */
  size_t node_size;          /* size of individual tree node in bytes */

  /*
   * workspace of size MAX(size1,size2)*MAX(sizeof(unsigned int),sizeof(int))
   * used in various routines
   */
  union
    {
      void *work_void;
      int *work_int;
      unsigned int *work_atomic;
    } work;

  int sptype;                /* sparse storage type */
  size_t spflags;            /* GSL_SPMATRIX_FLG_xxx */
} gsl_spmatrix_uint;

/*
 * Prototypes
 */

/* allocation / initialization */

gsl_spmatrix_uint * gsl_spmatrix_uint_alloc (const size_t n1, const size_t n2);
gsl_spmatrix_uint * gsl_spmatrix_uint_alloc_nzmax (const size_t n1, const size_t n2,
                                                     const size_t nzmax, const int sptype);
void gsl_spmatrix_uint_free (gsl_spmatrix_uint * m);
int gsl_spmatrix_uint_realloc (const size_t nzmax, gsl_spmatrix_uint * m);
size_t gsl_spmatrix_uint_nnz (const gsl_spmatrix_uint * m);
const char * gsl_spmatrix_uint_type (const gsl_spmatrix_uint * m);
int gsl_spmatrix_uint_set_zero (gsl_spmatrix_uint * m);
int gsl_spmatrix_uint_tree_rebuild (gsl_spmatrix_uint * m);

/* compress */

int gsl_spmatrix_uint_csc (gsl_spmatrix_uint * dest, const gsl_spmatrix_uint * src);
int gsl_spmatrix_uint_csr (gsl_spmatrix_uint * dest, const gsl_spmatrix_uint * src);
gsl_spmatrix_uint * gsl_spmatrix_uint_compress (const gsl_spmatrix_uint * src, const int sptype);
gsl_spmatrix_uint * gsl_spmatrix_uint_compcol (const gsl_spmatrix_uint * src);
gsl_spmatrix_uint * gsl_spmatrix_uint_ccs (const gsl_spmatrix_uint * src);
gsl_spmatrix_uint * gsl_spmatrix_uint_crs (const gsl_spmatrix_uint * src);

/* copy */

int gsl_spmatrix_uint_memcpy (gsl_spmatrix_uint * dest, const gsl_spmatrix_uint * src);

/* file I/O */

int gsl_spmatrix_uint_fprintf (FILE * stream, const gsl_spmatrix_uint * m, const char * format);
gsl_spmatrix_uint * gsl_spmatrix_uint_fscanf (FILE * stream);
int gsl_spmatrix_uint_fwrite (FILE * stream, const gsl_spmatrix_uint * m);
int gsl_spmatrix_uint_fread (FILE * stream, gsl_spmatrix_uint * m);

/* get/set */

unsigned int gsl_spmatrix_uint_get (const gsl_spmatrix_uint * m, const size_t i, const size_t j);
int gsl_spmatrix_uint_set (gsl_spmatrix_uint * m, const size_t i, const size_t j, const unsigned int x);
unsigned int * gsl_spmatrix_uint_ptr (const gsl_spmatrix_uint * m, const size_t i, const size_t j);

/* minmax */

int gsl_spmatrix_uint_minmax (const gsl_spmatrix_uint * m, unsigned int * min_out, unsigned int * max_out);
int gsl_spmatrix_uint_min_index (const gsl_spmatrix_uint * m, size_t * imin_out, size_t * jmin_out);

/* operations */

int gsl_spmatrix_uint_scale (gsl_spmatrix_uint * m, const unsigned int x);
int gsl_spmatrix_uint_scale_columns (gsl_spmatrix_uint * m, const gsl_vector_uint * x);
int gsl_spmatrix_uint_scale_rows (gsl_spmatrix_uint * m, const gsl_vector_uint * x);
int gsl_spmatrix_uint_add (gsl_spmatrix_uint * c, const gsl_spmatrix_uint * a, const gsl_spmatrix_uint * b);
int gsl_spmatrix_uint_dense_add (gsl_matrix_uint * a, const gsl_spmatrix_uint * b);
int gsl_spmatrix_uint_dense_sub (gsl_matrix_uint * a, const gsl_spmatrix_uint * b);
int gsl_spmatrix_uint_d2sp (gsl_spmatrix_uint * T, const gsl_matrix_uint * A);
int gsl_spmatrix_uint_sp2d (gsl_matrix_uint * A, const gsl_spmatrix_uint * S);

#ifndef GSL_DISABLE_DEPRECATED

int gsl_spmatrix_uint_add_to_dense (gsl_matrix_uint * a, const gsl_spmatrix_uint * b);

#endif

/* properties */

int gsl_spmatrix_uint_equal (const gsl_spmatrix_uint * a, const gsl_spmatrix_uint * b);
unsigned int gsl_spmatrix_uint_norm1 (const gsl_spmatrix_uint * a);

/* swap */

int gsl_spmatrix_uint_transpose (gsl_spmatrix_uint * m);
int gsl_spmatrix_uint_transpose2 (gsl_spmatrix_uint * m);
int gsl_spmatrix_uint_transpose_memcpy (gsl_spmatrix_uint * dest, const gsl_spmatrix_uint * src);

__END_DECLS

#endif /* __GSL_SPMATRIX_UINT_H__ */
