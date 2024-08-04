/* spmatrix/gsl_spmatrix_ushort.h
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

#ifndef __GSL_SPMATRIX_USHORT_H__
#define __GSL_SPMATRIX_USHORT_H__

#include <stdlib.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_bst.h>
#include <gsl/gsl_vector_ushort.h>
#include <gsl/gsl_matrix_ushort.h>

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

  unsigned short *data;               /* matrix elements of size nzmax */

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
   * workspace of size MAX(size1,size2)*MAX(sizeof(unsigned short),sizeof(int))
   * used in various routines
   */
  union
    {
      void *work_void;
      int *work_int;
      unsigned short *work_atomic;
    } work;

  int sptype;                /* sparse storage type */
  size_t spflags;            /* GSL_SPMATRIX_FLG_xxx */
} gsl_spmatrix_ushort;

/*
 * Prototypes
 */

/* allocation / initialization */

gsl_spmatrix_ushort * gsl_spmatrix_ushort_alloc (const size_t n1, const size_t n2);
gsl_spmatrix_ushort * gsl_spmatrix_ushort_alloc_nzmax (const size_t n1, const size_t n2,
                                                     const size_t nzmax, const int sptype);
void gsl_spmatrix_ushort_free (gsl_spmatrix_ushort * m);
int gsl_spmatrix_ushort_realloc (const size_t nzmax, gsl_spmatrix_ushort * m);
size_t gsl_spmatrix_ushort_nnz (const gsl_spmatrix_ushort * m);
const char * gsl_spmatrix_ushort_type (const gsl_spmatrix_ushort * m);
int gsl_spmatrix_ushort_set_zero (gsl_spmatrix_ushort * m);
int gsl_spmatrix_ushort_tree_rebuild (gsl_spmatrix_ushort * m);

/* compress */

int gsl_spmatrix_ushort_csc (gsl_spmatrix_ushort * dest, const gsl_spmatrix_ushort * src);
int gsl_spmatrix_ushort_csr (gsl_spmatrix_ushort * dest, const gsl_spmatrix_ushort * src);
gsl_spmatrix_ushort * gsl_spmatrix_ushort_compress (const gsl_spmatrix_ushort * src, const int sptype);
gsl_spmatrix_ushort * gsl_spmatrix_ushort_compcol (const gsl_spmatrix_ushort * src);
gsl_spmatrix_ushort * gsl_spmatrix_ushort_ccs (const gsl_spmatrix_ushort * src);
gsl_spmatrix_ushort * gsl_spmatrix_ushort_crs (const gsl_spmatrix_ushort * src);

/* copy */

int gsl_spmatrix_ushort_memcpy (gsl_spmatrix_ushort * dest, const gsl_spmatrix_ushort * src);

/* file I/O */

int gsl_spmatrix_ushort_fprintf (FILE * stream, const gsl_spmatrix_ushort * m, const char * format);
gsl_spmatrix_ushort * gsl_spmatrix_ushort_fscanf (FILE * stream);
int gsl_spmatrix_ushort_fwrite (FILE * stream, const gsl_spmatrix_ushort * m);
int gsl_spmatrix_ushort_fread (FILE * stream, gsl_spmatrix_ushort * m);

/* get/set */

unsigned short gsl_spmatrix_ushort_get (const gsl_spmatrix_ushort * m, const size_t i, const size_t j);
int gsl_spmatrix_ushort_set (gsl_spmatrix_ushort * m, const size_t i, const size_t j, const unsigned short x);
unsigned short * gsl_spmatrix_ushort_ptr (const gsl_spmatrix_ushort * m, const size_t i, const size_t j);

/* minmax */

int gsl_spmatrix_ushort_minmax (const gsl_spmatrix_ushort * m, unsigned short * min_out, unsigned short * max_out);
int gsl_spmatrix_ushort_min_index (const gsl_spmatrix_ushort * m, size_t * imin_out, size_t * jmin_out);

/* operations */

int gsl_spmatrix_ushort_scale (gsl_spmatrix_ushort * m, const unsigned short x);
int gsl_spmatrix_ushort_scale_columns (gsl_spmatrix_ushort * m, const gsl_vector_ushort * x);
int gsl_spmatrix_ushort_scale_rows (gsl_spmatrix_ushort * m, const gsl_vector_ushort * x);
int gsl_spmatrix_ushort_add (gsl_spmatrix_ushort * c, const gsl_spmatrix_ushort * a, const gsl_spmatrix_ushort * b);
int gsl_spmatrix_ushort_dense_add (gsl_matrix_ushort * a, const gsl_spmatrix_ushort * b);
int gsl_spmatrix_ushort_dense_sub (gsl_matrix_ushort * a, const gsl_spmatrix_ushort * b);
int gsl_spmatrix_ushort_d2sp (gsl_spmatrix_ushort * T, const gsl_matrix_ushort * A);
int gsl_spmatrix_ushort_sp2d (gsl_matrix_ushort * A, const gsl_spmatrix_ushort * S);

#ifndef GSL_DISABLE_DEPRECATED

int gsl_spmatrix_ushort_add_to_dense (gsl_matrix_ushort * a, const gsl_spmatrix_ushort * b);

#endif

/* properties */

int gsl_spmatrix_ushort_equal (const gsl_spmatrix_ushort * a, const gsl_spmatrix_ushort * b);
unsigned short gsl_spmatrix_ushort_norm1 (const gsl_spmatrix_ushort * a);

/* swap */

int gsl_spmatrix_ushort_transpose (gsl_spmatrix_ushort * m);
int gsl_spmatrix_ushort_transpose2 (gsl_spmatrix_ushort * m);
int gsl_spmatrix_ushort_transpose_memcpy (gsl_spmatrix_ushort * dest, const gsl_spmatrix_ushort * src);

__END_DECLS

#endif /* __GSL_SPMATRIX_USHORT_H__ */
