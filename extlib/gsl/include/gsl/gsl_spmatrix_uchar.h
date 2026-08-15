/* spmatrix/gsl_spmatrix_uchar.h
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

#ifndef __GSL_SPMATRIX_UCHAR_H__
#define __GSL_SPMATRIX_UCHAR_H__

#include <stdlib.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_bst.h>
#include <gsl/gsl_vector_uchar.h>
#include <gsl/gsl_matrix_uchar.h>

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

  unsigned char *data;               /* matrix elements of size nzmax */

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
   * workspace of size MAX(size1,size2)*MAX(sizeof(unsigned char),sizeof(int))
   * used in various routines
   */
  union
    {
      void *work_void;
      int *work_int;
      unsigned char *work_atomic;
    } work;

  int sptype;                /* sparse storage type */
  size_t spflags;            /* GSL_SPMATRIX_FLG_xxx */
} gsl_spmatrix_uchar;

/*
 * Prototypes
 */

/* allocation / initialization */

gsl_spmatrix_uchar * gsl_spmatrix_uchar_alloc (const size_t n1, const size_t n2);
gsl_spmatrix_uchar * gsl_spmatrix_uchar_alloc_nzmax (const size_t n1, const size_t n2,
                                                     const size_t nzmax, const int sptype);
void gsl_spmatrix_uchar_free (gsl_spmatrix_uchar * m);
int gsl_spmatrix_uchar_realloc (const size_t nzmax, gsl_spmatrix_uchar * m);
size_t gsl_spmatrix_uchar_nnz (const gsl_spmatrix_uchar * m);
const char * gsl_spmatrix_uchar_type (const gsl_spmatrix_uchar * m);
int gsl_spmatrix_uchar_set_zero (gsl_spmatrix_uchar * m);
int gsl_spmatrix_uchar_tree_rebuild (gsl_spmatrix_uchar * m);

/* compress */

int gsl_spmatrix_uchar_csc (gsl_spmatrix_uchar * dest, const gsl_spmatrix_uchar * src);
int gsl_spmatrix_uchar_csr (gsl_spmatrix_uchar * dest, const gsl_spmatrix_uchar * src);
gsl_spmatrix_uchar * gsl_spmatrix_uchar_compress (const gsl_spmatrix_uchar * src, const int sptype);
gsl_spmatrix_uchar * gsl_spmatrix_uchar_compcol (const gsl_spmatrix_uchar * src);
gsl_spmatrix_uchar * gsl_spmatrix_uchar_ccs (const gsl_spmatrix_uchar * src);
gsl_spmatrix_uchar * gsl_spmatrix_uchar_crs (const gsl_spmatrix_uchar * src);

/* copy */

int gsl_spmatrix_uchar_memcpy (gsl_spmatrix_uchar * dest, const gsl_spmatrix_uchar * src);

/* file I/O */

int gsl_spmatrix_uchar_fprintf (FILE * stream, const gsl_spmatrix_uchar * m, const char * format);
gsl_spmatrix_uchar * gsl_spmatrix_uchar_fscanf (FILE * stream);
int gsl_spmatrix_uchar_fwrite (FILE * stream, const gsl_spmatrix_uchar * m);
int gsl_spmatrix_uchar_fread (FILE * stream, gsl_spmatrix_uchar * m);

/* get/set */

unsigned char gsl_spmatrix_uchar_get (const gsl_spmatrix_uchar * m, const size_t i, const size_t j);
int gsl_spmatrix_uchar_set (gsl_spmatrix_uchar * m, const size_t i, const size_t j, const unsigned char x);
unsigned char * gsl_spmatrix_uchar_ptr (const gsl_spmatrix_uchar * m, const size_t i, const size_t j);

/* minmax */

int gsl_spmatrix_uchar_minmax (const gsl_spmatrix_uchar * m, unsigned char * min_out, unsigned char * max_out);
int gsl_spmatrix_uchar_min_index (const gsl_spmatrix_uchar * m, size_t * imin_out, size_t * jmin_out);

/* operations */

int gsl_spmatrix_uchar_scale (gsl_spmatrix_uchar * m, const unsigned char x);
int gsl_spmatrix_uchar_scale_columns (gsl_spmatrix_uchar * m, const gsl_vector_uchar * x);
int gsl_spmatrix_uchar_scale_rows (gsl_spmatrix_uchar * m, const gsl_vector_uchar * x);
int gsl_spmatrix_uchar_add (gsl_spmatrix_uchar * c, const gsl_spmatrix_uchar * a, const gsl_spmatrix_uchar * b);
int gsl_spmatrix_uchar_dense_add (gsl_matrix_uchar * a, const gsl_spmatrix_uchar * b);
int gsl_spmatrix_uchar_dense_sub (gsl_matrix_uchar * a, const gsl_spmatrix_uchar * b);
int gsl_spmatrix_uchar_d2sp (gsl_spmatrix_uchar * T, const gsl_matrix_uchar * A);
int gsl_spmatrix_uchar_sp2d (gsl_matrix_uchar * A, const gsl_spmatrix_uchar * S);

#ifndef GSL_DISABLE_DEPRECATED

int gsl_spmatrix_uchar_add_to_dense (gsl_matrix_uchar * a, const gsl_spmatrix_uchar * b);

#endif

/* properties */

int gsl_spmatrix_uchar_equal (const gsl_spmatrix_uchar * a, const gsl_spmatrix_uchar * b);
unsigned char gsl_spmatrix_uchar_norm1 (const gsl_spmatrix_uchar * a);

/* swap */

int gsl_spmatrix_uchar_transpose (gsl_spmatrix_uchar * m);
int gsl_spmatrix_uchar_transpose2 (gsl_spmatrix_uchar * m);
int gsl_spmatrix_uchar_transpose_memcpy (gsl_spmatrix_uchar * dest, const gsl_spmatrix_uchar * src);

__END_DECLS

#endif /* __GSL_SPMATRIX_UCHAR_H__ */
