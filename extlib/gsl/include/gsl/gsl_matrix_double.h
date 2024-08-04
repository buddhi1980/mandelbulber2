/* matrix/gsl_matrix_double.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Gerard Jungman, Brian Gough
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

#ifndef __GSL_MATRIX_DOUBLE_H__
#define __GSL_MATRIX_DOUBLE_H__

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
#include <gsl/gsl_types.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_inline.h>
#include <gsl/gsl_check_range.h>
#include <gsl/gsl_vector_double.h>

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

typedef struct 
{
  size_t size1;
  size_t size2;
  size_t tda;
  double * data;
  gsl_block * block;
  int owner;
} gsl_matrix;

typedef struct
{
  gsl_matrix matrix;
} _gsl_matrix_view;

typedef _gsl_matrix_view gsl_matrix_view;

typedef struct
{
  gsl_matrix matrix;
} _gsl_matrix_const_view;

typedef const _gsl_matrix_const_view gsl_matrix_const_view;

/* Allocation */

GSL_FUN gsl_matrix * 
gsl_matrix_alloc (const size_t n1, const size_t n2);

GSL_FUN gsl_matrix * 
gsl_matrix_calloc (const size_t n1, const size_t n2);

GSL_FUN gsl_matrix * 
gsl_matrix_alloc_from_block (gsl_block * b, 
                                   const size_t offset, 
                                   const size_t n1, 
                                   const size_t n2, 
                                   const size_t d2);

GSL_FUN gsl_matrix * 
gsl_matrix_alloc_from_matrix (gsl_matrix * m,
                                    const size_t k1, 
                                    const size_t k2,
                                    const size_t n1, 
                                    const size_t n2);

GSL_FUN gsl_vector * 
gsl_vector_alloc_row_from_matrix (gsl_matrix * m,
                                        const size_t i);

GSL_FUN gsl_vector * 
gsl_vector_alloc_col_from_matrix (gsl_matrix * m,
                                        const size_t j);

GSL_FUN void gsl_matrix_free (gsl_matrix * m);

/* Views */

GSL_FUN _gsl_matrix_view 
gsl_matrix_submatrix (gsl_matrix * m, 
                            const size_t i, const size_t j, 
                            const size_t n1, const size_t n2);

GSL_FUN _gsl_vector_view 
gsl_matrix_row (gsl_matrix * m, const size_t i);

GSL_FUN _gsl_vector_view 
gsl_matrix_column (gsl_matrix * m, const size_t j);

GSL_FUN _gsl_vector_view 
gsl_matrix_diagonal (gsl_matrix * m);

GSL_FUN _gsl_vector_view 
gsl_matrix_subdiagonal (gsl_matrix * m, const size_t k);

GSL_FUN _gsl_vector_view 
gsl_matrix_superdiagonal (gsl_matrix * m, const size_t k);

GSL_FUN _gsl_vector_view
gsl_matrix_subrow (gsl_matrix * m, const size_t i,
                         const size_t offset, const size_t n);

GSL_FUN _gsl_vector_view
gsl_matrix_subcolumn (gsl_matrix * m, const size_t j,
                            const size_t offset, const size_t n);

GSL_FUN _gsl_matrix_view
gsl_matrix_view_array (double * base,
                             const size_t n1, 
                             const size_t n2);

GSL_FUN _gsl_matrix_view
gsl_matrix_view_array_with_tda (double * base, 
                                      const size_t n1, 
                                      const size_t n2,
                                      const size_t tda);


GSL_FUN _gsl_matrix_view
gsl_matrix_view_vector (gsl_vector * v,
                              const size_t n1, 
                              const size_t n2);

GSL_FUN _gsl_matrix_view
gsl_matrix_view_vector_with_tda (gsl_vector * v,
                                       const size_t n1, 
                                       const size_t n2,
                                       const size_t tda);


GSL_FUN _gsl_matrix_const_view 
gsl_matrix_const_submatrix (const gsl_matrix * m, 
                                  const size_t i, const size_t j, 
                                  const size_t n1, const size_t n2);

GSL_FUN _gsl_vector_const_view 
gsl_matrix_const_row (const gsl_matrix * m, 
                            const size_t i);

GSL_FUN _gsl_vector_const_view 
gsl_matrix_const_column (const gsl_matrix * m, 
                               const size_t j);

GSL_FUN _gsl_vector_const_view
gsl_matrix_const_diagonal (const gsl_matrix * m);

GSL_FUN _gsl_vector_const_view 
gsl_matrix_const_subdiagonal (const gsl_matrix * m, 
                                    const size_t k);

GSL_FUN _gsl_vector_const_view 
gsl_matrix_const_superdiagonal (const gsl_matrix * m, 
                                      const size_t k);

GSL_FUN _gsl_vector_const_view
gsl_matrix_const_subrow (const gsl_matrix * m, const size_t i,
                               const size_t offset, const size_t n);

GSL_FUN _gsl_vector_const_view
gsl_matrix_const_subcolumn (const gsl_matrix * m, const size_t j,
                                  const size_t offset, const size_t n);

GSL_FUN _gsl_matrix_const_view
gsl_matrix_const_view_array (const double * base,
                                   const size_t n1, 
                                   const size_t n2);

GSL_FUN _gsl_matrix_const_view
gsl_matrix_const_view_array_with_tda (const double * base, 
                                            const size_t n1, 
                                            const size_t n2,
                                            const size_t tda);

GSL_FUN _gsl_matrix_const_view
gsl_matrix_const_view_vector (const gsl_vector * v,
                                    const size_t n1, 
                                    const size_t n2);

GSL_FUN _gsl_matrix_const_view
gsl_matrix_const_view_vector_with_tda (const gsl_vector * v,
                                             const size_t n1, 
                                             const size_t n2,
                                             const size_t tda);

/* Operations */

GSL_FUN void gsl_matrix_set_zero (gsl_matrix * m);
GSL_FUN void gsl_matrix_set_identity (gsl_matrix * m);
GSL_FUN void gsl_matrix_set_all (gsl_matrix * m, double x);

GSL_FUN int gsl_matrix_fread (FILE * stream, gsl_matrix * m) ;
GSL_FUN int gsl_matrix_fwrite (FILE * stream, const gsl_matrix * m) ;
GSL_FUN int gsl_matrix_fscanf (FILE * stream, gsl_matrix * m);
GSL_FUN int gsl_matrix_fprintf (FILE * stream, const gsl_matrix * m, const char * format);
 
GSL_FUN int gsl_matrix_memcpy(gsl_matrix * dest, const gsl_matrix * src);
GSL_FUN int gsl_matrix_swap(gsl_matrix * m1, gsl_matrix * m2);
GSL_FUN int gsl_matrix_tricpy(const char uplo_src, const int copy_diag, gsl_matrix * dest, const gsl_matrix * src);

GSL_FUN int gsl_matrix_swap_rows(gsl_matrix * m, const size_t i, const size_t j);
GSL_FUN int gsl_matrix_swap_columns(gsl_matrix * m, const size_t i, const size_t j);
GSL_FUN int gsl_matrix_swap_rowcol(gsl_matrix * m, const size_t i, const size_t j);
GSL_FUN int gsl_matrix_transpose (gsl_matrix * m);
GSL_FUN int gsl_matrix_transpose_memcpy (gsl_matrix * dest, const gsl_matrix * src);
GSL_FUN int gsl_matrix_transpose_tricpy (const char uplo_src, const int copy_diag, gsl_matrix * dest, const gsl_matrix * src);

GSL_FUN double gsl_matrix_max (const gsl_matrix * m);
GSL_FUN double gsl_matrix_min (const gsl_matrix * m);
GSL_FUN void gsl_matrix_minmax (const gsl_matrix * m, double * min_out, double * max_out);

GSL_FUN void gsl_matrix_max_index (const gsl_matrix * m, size_t * imax, size_t *jmax);
GSL_FUN void gsl_matrix_min_index (const gsl_matrix * m, size_t * imin, size_t *jmin);
GSL_FUN void gsl_matrix_minmax_index (const gsl_matrix * m, size_t * imin, size_t * jmin, size_t * imax, size_t * jmax);

GSL_FUN int gsl_matrix_equal (const gsl_matrix * a, const gsl_matrix * b);

GSL_FUN int gsl_matrix_isnull (const gsl_matrix * m);
GSL_FUN int gsl_matrix_ispos (const gsl_matrix * m);
GSL_FUN int gsl_matrix_isneg (const gsl_matrix * m);
GSL_FUN int gsl_matrix_isnonneg (const gsl_matrix * m);

GSL_FUN int gsl_matrix_add (gsl_matrix * a, const gsl_matrix * b);
GSL_FUN int gsl_matrix_sub (gsl_matrix * a, const gsl_matrix * b);
GSL_FUN int gsl_matrix_mul_elements (gsl_matrix * a, const gsl_matrix * b);
GSL_FUN int gsl_matrix_div_elements (gsl_matrix * a, const gsl_matrix * b);
GSL_FUN int gsl_matrix_scale (gsl_matrix * a, const double x);
GSL_FUN int gsl_matrix_add_constant (gsl_matrix * a, const double x);
GSL_FUN int gsl_matrix_add_diagonal (gsl_matrix * a, const double x);

/***********************************************************************/
/* The functions below are obsolete                                    */
/***********************************************************************/
GSL_FUN int gsl_matrix_get_row(gsl_vector * v, const gsl_matrix * m, const size_t i);
GSL_FUN int gsl_matrix_get_col(gsl_vector * v, const gsl_matrix * m, const size_t j);
GSL_FUN int gsl_matrix_set_row(gsl_matrix * m, const size_t i, const gsl_vector * v);
GSL_FUN int gsl_matrix_set_col(gsl_matrix * m, const size_t j, const gsl_vector * v);
/***********************************************************************/

/* inline functions if you are using GCC */

GSL_FUN INLINE_DECL double   gsl_matrix_get(const gsl_matrix * m, const size_t i, const size_t j);
GSL_FUN INLINE_DECL void    gsl_matrix_set(gsl_matrix * m, const size_t i, const size_t j, const double x);
GSL_FUN INLINE_DECL double * gsl_matrix_ptr(gsl_matrix * m, const size_t i, const size_t j);
GSL_FUN INLINE_DECL const double * gsl_matrix_const_ptr(const gsl_matrix * m, const size_t i, const size_t j);

#ifdef HAVE_INLINE
INLINE_FUN 
double
gsl_matrix_get(const gsl_matrix * m, const size_t i, const size_t j)
{
#if GSL_RANGE_CHECK
  if (GSL_RANGE_COND(1)) 
    {
      if (i >= m->size1)
        {
          GSL_ERROR_VAL("first index out of range", GSL_EINVAL, 0) ;
        }
      else if (j >= m->size2)
        {
          GSL_ERROR_VAL("second index out of range", GSL_EINVAL, 0) ;
        }
    }
#endif
  return m->data[i * m->tda + j] ;
} 

INLINE_FUN 
void
gsl_matrix_set(gsl_matrix * m, const size_t i, const size_t j, const double x)
{
#if GSL_RANGE_CHECK
  if (GSL_RANGE_COND(1)) 
    {
      if (i >= m->size1)
        {
          GSL_ERROR_VOID("first index out of range", GSL_EINVAL) ;
        }
      else if (j >= m->size2)
        {
          GSL_ERROR_VOID("second index out of range", GSL_EINVAL) ;
        }
    }
#endif
  m->data[i * m->tda + j] = x ;
}

INLINE_FUN 
double *
gsl_matrix_ptr(gsl_matrix * m, const size_t i, const size_t j)
{
#if GSL_RANGE_CHECK
  if (GSL_RANGE_COND(1)) 
    {
      if (i >= m->size1)
        {
          GSL_ERROR_NULL("first index out of range", GSL_EINVAL) ;
        }
      else if (j >= m->size2)
        {
          GSL_ERROR_NULL("second index out of range", GSL_EINVAL) ;
        }
    }
#endif
  return (double *) (m->data + (i * m->tda + j)) ;
} 

INLINE_FUN 
const double *
gsl_matrix_const_ptr(const gsl_matrix * m, const size_t i, const size_t j)
{
#if GSL_RANGE_CHECK
  if (GSL_RANGE_COND(1)) 
    {
      if (i >= m->size1)
        {
          GSL_ERROR_NULL("first index out of range", GSL_EINVAL) ;
        }
      else if (j >= m->size2)
        {
          GSL_ERROR_NULL("second index out of range", GSL_EINVAL) ;
        }
    }
#endif
  return (const double *) (m->data + (i * m->tda + j)) ;
} 

#endif

__END_DECLS

#endif /* __GSL_MATRIX_DOUBLE_H__ */
