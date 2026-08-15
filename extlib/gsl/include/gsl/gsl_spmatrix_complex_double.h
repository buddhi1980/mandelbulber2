/* spmatrix/gsl_spmatrix_double.h
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

#ifndef __GSL_SPMATRIX_COMPLEX_DOUBLE_H__
#define __GSL_SPMATRIX_COMPLEX_DOUBLE_H__

#include <stdlib.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_bst.h>
#include <gsl/gsl_vector_complex_double.h>
#include <gsl/gsl_matrix_complex_double.h>

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
  size_t size1;              /* number of rows */
  size_t size2;              /* number of columns */

  /* i (size nzmax) contains:
   *
   * COO/CSC: row indices
   * CSR: column indices
   */
  int *i;

  double *data;               /* matrix elements of size nzmax */

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
   * workspace of size 2*MAX(size1,size2)*MAX(sizeof(double),sizeof(int))
   * used in various routines
   */
  union
    {
      void *work_void;
      int *work_int;
      double *work_atomic;
    } work;

  int sptype;                /* sparse storage type */
  size_t spflags;            /* GSL_SPMATRIX_FLG_xxx */
} gsl_spmatrix_complex;

/*
 * Prototypes
 */

/* allocation / initialization */

gsl_spmatrix_complex * gsl_spmatrix_complex_alloc (const size_t n1, const size_t n2);
gsl_spmatrix_complex * gsl_spmatrix_complex_alloc_nzmax (const size_t n1, const size_t n2,
                                                                     const size_t nzmax, const int sptype);
void gsl_spmatrix_complex_free (gsl_spmatrix_complex * m);
int gsl_spmatrix_complex_realloc (const size_t nzmax, gsl_spmatrix_complex * m);
size_t gsl_spmatrix_complex_nnz (const gsl_spmatrix_complex * m);
const char * gsl_spmatrix_complex_type (const gsl_spmatrix_complex * m);
int gsl_spmatrix_complex_set_zero (gsl_spmatrix_complex * m);
int gsl_spmatrix_complex_tree_rebuild (gsl_spmatrix_complex * m);

/* compress */

int gsl_spmatrix_complex_csc (gsl_spmatrix_complex * dest, const gsl_spmatrix_complex * src);
int gsl_spmatrix_complex_csr (gsl_spmatrix_complex * dest, const gsl_spmatrix_complex * src);
gsl_spmatrix_complex * gsl_spmatrix_complex_compress (const gsl_spmatrix_complex * src, const int sptype);
gsl_spmatrix_complex * gsl_spmatrix_complex_compcol (const gsl_spmatrix_complex * src);
gsl_spmatrix_complex * gsl_spmatrix_complex_ccs (const gsl_spmatrix_complex * src);
gsl_spmatrix_complex * gsl_spmatrix_complex_crs (const gsl_spmatrix_complex * src);

/* copy */

int gsl_spmatrix_complex_memcpy (gsl_spmatrix_complex * dest, const gsl_spmatrix_complex * src);

/* file I/O */

int gsl_spmatrix_complex_fprintf (FILE * stream, const gsl_spmatrix_complex * m, const char * format);
gsl_spmatrix_complex * gsl_spmatrix_complex_fscanf (FILE * stream);
int gsl_spmatrix_complex_fwrite (FILE * stream, const gsl_spmatrix_complex * m);
int gsl_spmatrix_complex_fread (FILE * stream, gsl_spmatrix_complex * m);

/* get/set */

gsl_complex gsl_spmatrix_complex_get (const gsl_spmatrix_complex * m, const size_t i, const size_t j);
int gsl_spmatrix_complex_set (gsl_spmatrix_complex * m, const size_t i, const size_t j, const gsl_complex x);
gsl_complex * gsl_spmatrix_complex_ptr (const gsl_spmatrix_complex * m, const size_t i, const size_t j);

/* operations */

int gsl_spmatrix_complex_scale (gsl_spmatrix_complex * m, const gsl_complex x);
int gsl_spmatrix_complex_scale_columns (gsl_spmatrix_complex * m, const gsl_vector_complex * x);
int gsl_spmatrix_complex_scale_rows (gsl_spmatrix_complex * m, const gsl_vector_complex * x);
int gsl_spmatrix_complex_add (gsl_spmatrix_complex * c, const gsl_spmatrix_complex * a, const gsl_spmatrix_complex * b);
int gsl_spmatrix_complex_dense_add (gsl_matrix_complex * a, const gsl_spmatrix_complex * b);
int gsl_spmatrix_complex_dense_sub (gsl_matrix_complex * a, const gsl_spmatrix_complex * b);
int gsl_spmatrix_complex_d2sp (gsl_spmatrix_complex * T, const gsl_matrix_complex * A);
int gsl_spmatrix_complex_sp2d (gsl_matrix_complex * A, const gsl_spmatrix_complex * S);

#ifndef GSL_DISABLE_DEPRECATED

int gsl_spmatrix_complex_add_to_dense (gsl_matrix_complex * a, const gsl_spmatrix_complex * b);

#endif

/* properties */

int gsl_spmatrix_complex_equal (const gsl_spmatrix_complex * a, const gsl_spmatrix_complex * b);

/* swap */

int gsl_spmatrix_complex_transpose (gsl_spmatrix_complex * m);
int gsl_spmatrix_complex_transpose2 (gsl_spmatrix_complex * m);
int gsl_spmatrix_complex_transpose_memcpy (gsl_spmatrix_complex * dest, const gsl_spmatrix_complex * src);

__END_DECLS

#endif /* __GSL_SPMATRIX_COMPLEX_DOUBLE_H__ */
