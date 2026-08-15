/* gsl_spmatrix.h
 * 
 * Copyright (C) 2012-2014 Patrick Alken
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

#ifndef __GSL_SPMATRIX_H__
#define __GSL_SPMATRIX_H__

#if !defined( GSL_FUN )
#  if !defined( GSL_DLL )
#    define GSL_FUN extern
#  elif defined( BUILD_GSL_DLL )
#    define GSL_FUN extern __declspec(dllexport)
#  else
#    define GSL_FUN extern __declspec(dllimport)
#  endif
#endif

#include <stdlib.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

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
 * Binary tree data structure for storing sparse matrix elements
 * in triplet format. This is used for efficiently detecting
 * duplicates and element retrieval via gsl_spmatrix_get
 */
typedef struct
{
  void *tree;       /* tree structure */
  void *node_array; /* preallocated array of tree nodes */
  size_t n;         /* number of tree nodes in use (<= nzmax) */
} gsl_spmatrix_tree;

/*
 * Triplet format:
 *
 * If data[n] = A_{ij}, then:
 *   i = A->i[n]
 *   j = A->p[n]
 *
 * Compressed column format (CCS):
 *
 * If data[n] = A_{ij}, then:
 *   i = A->i[n]
 *   A->p[j] <= n < A->p[j+1]
 * so that column j is stored in
 * [ data[p[j]], data[p[j] + 1], ..., data[p[j+1] - 1] ]
 *
 * Compressed row format (CRS):
 *
 * If data[n] = A_{ij}, then:
 *   j = A->i[n]
 *   A->p[i] <= n < A->p[i+1]
 * so that row i is stored in
 * [ data[p[i]], data[p[i] + 1], ..., data[p[i+1] - 1] ]
 */

typedef struct
{
  size_t size1;  /* number of rows */
  size_t size2;  /* number of columns */

  /* i (size nzmax) contains:
   *
   * Triplet/CCS: row indices
   * CRS: column indices
   */
  size_t *i;

  double *data;  /* matrix elements of size nzmax */

  /*
   * p contains the column indices (triplet) or column pointers (compcol)
   *
   * triplet: p[n] = column number of element data[n]
   * CCS:     p[j] = index in data of first non-zero element in column j
   * CRS:     p[i] = index in data of first non-zero element in row i
   */
  size_t *p;

  size_t nzmax;  /* maximum number of matrix elements */
  size_t nz;     /* number of non-zero values in matrix */

  gsl_spmatrix_tree *tree_data; /* binary tree for sorting triplet data */

  /*
   * workspace of size MAX(size1,size2)*MAX(sizeof(double),sizeof(size_t))
   * used in various routines
   */
  void *work;

  size_t sptype; /* sparse storage type */
} gsl_spmatrix;

#define GSL_SPMATRIX_TRIPLET      (0)
#define GSL_SPMATRIX_CCS          (1)
#define GSL_SPMATRIX_CRS          (2)

#define GSL_SPMATRIX_ISTRIPLET(m) ((m)->sptype == GSL_SPMATRIX_TRIPLET)
#define GSL_SPMATRIX_ISCCS(m)     ((m)->sptype == GSL_SPMATRIX_CCS)
#define GSL_SPMATRIX_ISCRS(m)     ((m)->sptype == GSL_SPMATRIX_CRS)

/*
 * Prototypes
 */

GSL_FUN gsl_spmatrix *gsl_spmatrix_alloc(const size_t n1, const size_t n2);
GSL_FUN gsl_spmatrix *gsl_spmatrix_alloc_nzmax(const size_t n1, const size_t n2,
                                       const size_t nzmax, const size_t flags);
GSL_FUN void gsl_spmatrix_free(gsl_spmatrix *m);
GSL_FUN int gsl_spmatrix_realloc(const size_t nzmax, gsl_spmatrix *m);
GSL_FUN int gsl_spmatrix_set_zero(gsl_spmatrix *m);
GSL_FUN size_t gsl_spmatrix_nnz(const gsl_spmatrix *m);
GSL_FUN int gsl_spmatrix_compare_idx(const size_t ia, const size_t ja,
                             const size_t ib, const size_t jb);
GSL_FUN int gsl_spmatrix_tree_rebuild(gsl_spmatrix * m);

/* spcopy.c */
GSL_FUN int gsl_spmatrix_memcpy(gsl_spmatrix *dest, const gsl_spmatrix *src);

/* spgetset.c */
GSL_FUN double gsl_spmatrix_get(const gsl_spmatrix *m, const size_t i,
                        const size_t j);
GSL_FUN int gsl_spmatrix_set(gsl_spmatrix *m, const size_t i, const size_t j,
                     const double x);
GSL_FUN double *gsl_spmatrix_ptr(gsl_spmatrix *m, const size_t i, const size_t j);

/* spcompress.c */
GSL_FUN gsl_spmatrix *gsl_spmatrix_compcol(const gsl_spmatrix *T);
GSL_FUN gsl_spmatrix *gsl_spmatrix_ccs(const gsl_spmatrix *T);
GSL_FUN gsl_spmatrix *gsl_spmatrix_crs(const gsl_spmatrix *T);
GSL_FUN void gsl_spmatrix_cumsum(const size_t n, size_t *c);

/* spio.c */
GSL_FUN int gsl_spmatrix_fprintf(FILE *stream, const gsl_spmatrix *m,
                         const char *format);
GSL_FUN gsl_spmatrix * gsl_spmatrix_fscanf(FILE *stream);
GSL_FUN int gsl_spmatrix_fwrite(FILE *stream, const gsl_spmatrix *m);
GSL_FUN int gsl_spmatrix_fread(FILE *stream, gsl_spmatrix *m);

/* spoper.c */
GSL_FUN int gsl_spmatrix_scale(gsl_spmatrix *m, const double x);
GSL_FUN int gsl_spmatrix_minmax(const gsl_spmatrix *m, double *min_out,
                        double *max_out);
GSL_FUN int gsl_spmatrix_add(gsl_spmatrix *c, const gsl_spmatrix *a,
                     const gsl_spmatrix *b);
GSL_FUN int gsl_spmatrix_d2sp(gsl_spmatrix *S, const gsl_matrix *A);
GSL_FUN int gsl_spmatrix_sp2d(gsl_matrix *A, const gsl_spmatrix *S);

/* spprop.c */
GSL_FUN int gsl_spmatrix_equal(const gsl_spmatrix *a, const gsl_spmatrix *b);

/* spswap.c */
GSL_FUN int gsl_spmatrix_transpose(gsl_spmatrix * m);
GSL_FUN int gsl_spmatrix_transpose2(gsl_spmatrix * m);
GSL_FUN int gsl_spmatrix_transpose_memcpy(gsl_spmatrix *dest, const gsl_spmatrix *src);

__END_DECLS

#endif /* __GSL_SPMATRIX_H__ */
