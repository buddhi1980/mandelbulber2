/* spmatrix/gsl_spmatrix_float.h
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

#ifndef __GSL_SPMATRIX_FLOAT_H__
#define __GSL_SPMATRIX_FLOAT_H__

#include <stdlib.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_bst.h>
#include <gsl/gsl_vector_float.h>
#include <gsl/gsl_matrix_float.h>

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

  float *data;               /* matrix elements of size nzmax */

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
   * workspace of size MAX(size1,size2)*MAX(sizeof(float),sizeof(int))
   * used in various routines
   */
  union
    {
      void *work_void;
      int *work_int;
      float *work_atomic;
    } work;

  int sptype;                /* sparse storage type */
  size_t spflags;            /* GSL_SPMATRIX_FLG_xxx */
} gsl_spmatrix_float;

/*
 * Prototypes
 */

/* allocation / initialization */

gsl_spmatrix_float * gsl_spmatrix_float_alloc (const size_t n1, const size_t n2);
gsl_spmatrix_float * gsl_spmatrix_float_alloc_nzmax (const size_t n1, const size_t n2,
                                                     const size_t nzmax, const int sptype);
void gsl_spmatrix_float_free (gsl_spmatrix_float * m);
int gsl_spmatrix_float_realloc (const size_t nzmax, gsl_spmatrix_float * m);
size_t gsl_spmatrix_float_nnz (const gsl_spmatrix_float * m);
const char * gsl_spmatrix_float_type (const gsl_spmatrix_float * m);
int gsl_spmatrix_float_set_zero (gsl_spmatrix_float * m);
int gsl_spmatrix_float_tree_rebuild (gsl_spmatrix_float * m);

/* compress */

int gsl_spmatrix_float_csc (gsl_spmatrix_float * dest, const gsl_spmatrix_float * src);
int gsl_spmatrix_float_csr (gsl_spmatrix_float * dest, const gsl_spmatrix_float * src);
gsl_spmatrix_float * gsl_spmatrix_float_compress (const gsl_spmatrix_float * src, const int sptype);
gsl_spmatrix_float * gsl_spmatrix_float_compcol (const gsl_spmatrix_float * src);
gsl_spmatrix_float * gsl_spmatrix_float_ccs (const gsl_spmatrix_float * src);
gsl_spmatrix_float * gsl_spmatrix_float_crs (const gsl_spmatrix_float * src);

/* copy */

int gsl_spmatrix_float_memcpy (gsl_spmatrix_float * dest, const gsl_spmatrix_float * src);

/* file I/O */

int gsl_spmatrix_float_fprintf (FILE * stream, const gsl_spmatrix_float * m, const char * format);
gsl_spmatrix_float * gsl_spmatrix_float_fscanf (FILE * stream);
int gsl_spmatrix_float_fwrite (FILE * stream, const gsl_spmatrix_float * m);
int gsl_spmatrix_float_fread (FILE * stream, gsl_spmatrix_float * m);

/* get/set */

float gsl_spmatrix_float_get (const gsl_spmatrix_float * m, const size_t i, const size_t j);
int gsl_spmatrix_float_set (gsl_spmatrix_float * m, const size_t i, const size_t j, const float x);
float * gsl_spmatrix_float_ptr (const gsl_spmatrix_float * m, const size_t i, const size_t j);

/* minmax */

int gsl_spmatrix_float_minmax (const gsl_spmatrix_float * m, float * min_out, float * max_out);
int gsl_spmatrix_float_min_index (const gsl_spmatrix_float * m, size_t * imin_out, size_t * jmin_out);

/* operations */

int gsl_spmatrix_float_scale (gsl_spmatrix_float * m, const float x);
int gsl_spmatrix_float_scale_columns (gsl_spmatrix_float * m, const gsl_vector_float * x);
int gsl_spmatrix_float_scale_rows (gsl_spmatrix_float * m, const gsl_vector_float * x);
int gsl_spmatrix_float_add (gsl_spmatrix_float * c, const gsl_spmatrix_float * a, const gsl_spmatrix_float * b);
int gsl_spmatrix_float_dense_add (gsl_matrix_float * a, const gsl_spmatrix_float * b);
int gsl_spmatrix_float_dense_sub (gsl_matrix_float * a, const gsl_spmatrix_float * b);
int gsl_spmatrix_float_d2sp (gsl_spmatrix_float * T, const gsl_matrix_float * A);
int gsl_spmatrix_float_sp2d (gsl_matrix_float * A, const gsl_spmatrix_float * S);

#ifndef GSL_DISABLE_DEPRECATED

int gsl_spmatrix_float_add_to_dense (gsl_matrix_float * a, const gsl_spmatrix_float * b);

#endif

/* properties */

int gsl_spmatrix_float_equal (const gsl_spmatrix_float * a, const gsl_spmatrix_float * b);
float gsl_spmatrix_float_norm1 (const gsl_spmatrix_float * a);

/* swap */

int gsl_spmatrix_float_transpose (gsl_spmatrix_float * m);
int gsl_spmatrix_float_transpose2 (gsl_spmatrix_float * m);
int gsl_spmatrix_float_transpose_memcpy (gsl_spmatrix_float * dest, const gsl_spmatrix_float * src);

__END_DECLS

#endif /* __GSL_SPMATRIX_FLOAT_H__ */
