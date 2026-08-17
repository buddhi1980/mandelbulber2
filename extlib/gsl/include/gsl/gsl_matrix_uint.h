/* matrix/gsl_matrix_uint.h
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

#ifndef __GSL_MATRIX_UINT_H__
#define __GSL_MATRIX_UINT_H__

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
#include <gsl/gsl_vector_uint.h>

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
  unsigned int * data;
  gsl_block_uint * block;
  int owner;
} gsl_matrix_uint;

typedef struct
{
  gsl_matrix_uint matrix;
} _gsl_matrix_uint_view;

typedef _gsl_matrix_uint_view gsl_matrix_uint_view;

typedef struct
{
  gsl_matrix_uint matrix;
} _gsl_matrix_uint_const_view;

typedef const _gsl_matrix_uint_const_view gsl_matrix_uint_const_view;

/* Allocation */

GSL_FUN gsl_matrix_uint * 
gsl_matrix_uint_alloc (const size_t n1, const size_t n2);

GSL_FUN gsl_matrix_uint * 
gsl_matrix_uint_calloc (const size_t n1, const size_t n2);

GSL_FUN gsl_matrix_uint * 
gsl_matrix_uint_alloc_from_block (gsl_block_uint * b, 
                                   const size_t offset, 
                                   const size_t n1, 
                                   const size_t n2, 
                                   const size_t d2);

GSL_FUN gsl_matrix_uint * 
gsl_matrix_uint_alloc_from_matrix (gsl_matrix_uint * m,
                                    const size_t k1, 
                                    const size_t k2,
                                    const size_t n1, 
                                    const size_t n2);

GSL_FUN gsl_vector_uint * 
gsl_vector_uint_alloc_row_from_matrix (gsl_matrix_uint * m,
                                        const size_t i);

GSL_FUN gsl_vector_uint * 
gsl_vector_uint_alloc_col_from_matrix (gsl_matrix_uint * m,
                                        const size_t j);

GSL_FUN void gsl_matrix_uint_free (gsl_matrix_uint * m);

/* Views */

GSL_FUN _gsl_matrix_uint_view 
gsl_matrix_uint_submatrix (gsl_matrix_uint * m, 
                            const size_t i, const size_t j, 
                            const size_t n1, const size_t n2);

GSL_FUN _gsl_vector_uint_view 
gsl_matrix_uint_row (gsl_matrix_uint * m, const size_t i);

GSL_FUN _gsl_vector_uint_view 
gsl_matrix_uint_column (gsl_matrix_uint * m, const size_t j);

GSL_FUN _gsl_vector_uint_view 
gsl_matrix_uint_diagonal (gsl_matrix_uint * m);

GSL_FUN _gsl_vector_uint_view 
gsl_matrix_uint_subdiagonal (gsl_matrix_uint * m, const size_t k);

GSL_FUN _gsl_vector_uint_view 
gsl_matrix_uint_superdiagonal (gsl_matrix_uint * m, const size_t k);

GSL_FUN _gsl_vector_uint_view
gsl_matrix_uint_subrow (gsl_matrix_uint * m, const size_t i,
                         const size_t offset, const size_t n);

GSL_FUN _gsl_vector_uint_view
gsl_matrix_uint_subcolumn (gsl_matrix_uint * m, const size_t j,
                            const size_t offset, const size_t n);

GSL_FUN _gsl_matrix_uint_view
gsl_matrix_uint_view_array (unsigned int * base,
                             const size_t n1, 
                             const size_t n2);

GSL_FUN _gsl_matrix_uint_view
gsl_matrix_uint_view_array_with_tda (unsigned int * base, 
                                      const size_t n1, 
                                      const size_t n2,
                                      const size_t tda);


GSL_FUN _gsl_matrix_uint_view
gsl_matrix_uint_view_vector (gsl_vector_uint * v,
                              const size_t n1, 
                              const size_t n2);

GSL_FUN _gsl_matrix_uint_view
gsl_matrix_uint_view_vector_with_tda (gsl_vector_uint * v,
                                       const size_t n1, 
                                       const size_t n2,
                                       const size_t tda);


GSL_FUN _gsl_matrix_uint_const_view 
gsl_matrix_uint_const_submatrix (const gsl_matrix_uint * m, 
                                  const size_t i, const size_t j, 
                                  const size_t n1, const size_t n2);

GSL_FUN _gsl_vector_uint_const_view 
gsl_matrix_uint_const_row (const gsl_matrix_uint * m, 
                            const size_t i);

GSL_FUN _gsl_vector_uint_const_view 
gsl_matrix_uint_const_column (const gsl_matrix_uint * m, 
                               const size_t j);

GSL_FUN _gsl_vector_uint_const_view
gsl_matrix_uint_const_diagonal (const gsl_matrix_uint * m);

GSL_FUN _gsl_vector_uint_const_view 
gsl_matrix_uint_const_subdiagonal (const gsl_matrix_uint * m, 
                                    const size_t k);

GSL_FUN _gsl_vector_uint_const_view 
gsl_matrix_uint_const_superdiagonal (const gsl_matrix_uint * m, 
                                      const size_t k);

GSL_FUN _gsl_vector_uint_const_view
gsl_matrix_uint_const_subrow (const gsl_matrix_uint * m, const size_t i,
                               const size_t offset, const size_t n);

GSL_FUN _gsl_vector_uint_const_view
gsl_matrix_uint_const_subcolumn (const gsl_matrix_uint * m, const size_t j,
                                  const size_t offset, const size_t n);

GSL_FUN _gsl_matrix_uint_const_view
gsl_matrix_uint_const_view_array (const unsigned int * base,
                                   const size_t n1, 
                                   const size_t n2);

GSL_FUN _gsl_matrix_uint_const_view
gsl_matrix_uint_const_view_array_with_tda (const unsigned int * base, 
                                            const size_t n1, 
                                            const size_t n2,
                                            const size_t tda);

GSL_FUN _gsl_matrix_uint_const_view
gsl_matrix_uint_const_view_vector (const gsl_vector_uint * v,
                                    const size_t n1, 
                                    const size_t n2);

GSL_FUN _gsl_matrix_uint_const_view
gsl_matrix_uint_const_view_vector_with_tda (const gsl_vector_uint * v,
                                             const size_t n1, 
                                             const size_t n2,
                                             const size_t tda);

/* Operations */

GSL_FUN void gsl_matrix_uint_set_zero (gsl_matrix_uint * m);
GSL_FUN void gsl_matrix_uint_set_identity (gsl_matrix_uint * m);
GSL_FUN void gsl_matrix_uint_set_all (gsl_matrix_uint * m, unsigned int x);

GSL_FUN int gsl_matrix_uint_fread (FILE * stream, gsl_matrix_uint * m) ;
GSL_FUN int gsl_matrix_uint_fwrite (FILE * stream, const gsl_matrix_uint * m) ;
GSL_FUN int gsl_matrix_uint_fscanf (FILE * stream, gsl_matrix_uint * m);
GSL_FUN int gsl_matrix_uint_fprintf (FILE * stream, const gsl_matrix_uint * m, const char * format);
 
GSL_FUN int gsl_matrix_uint_memcpy(gsl_matrix_uint * dest, const gsl_matrix_uint * src);
GSL_FUN int gsl_matrix_uint_swap(gsl_matrix_uint * m1, gsl_matrix_uint * m2);
GSL_FUN int gsl_matrix_uint_tricpy(const char uplo_src, const int copy_diag, gsl_matrix_uint * dest, const gsl_matrix_uint * src);

GSL_FUN int gsl_matrix_uint_swap_rows(gsl_matrix_uint * m, const size_t i, const size_t j);
GSL_FUN int gsl_matrix_uint_swap_columns(gsl_matrix_uint * m, const size_t i, const size_t j);
GSL_FUN int gsl_matrix_uint_swap_rowcol(gsl_matrix_uint * m, const size_t i, const size_t j);
GSL_FUN int gsl_matrix_uint_transpose (gsl_matrix_uint * m);
GSL_FUN int gsl_matrix_uint_transpose_memcpy (gsl_matrix_uint * dest, const gsl_matrix_uint * src);
GSL_FUN int gsl_matrix_uint_transpose_tricpy (const char uplo_src, const int copy_diag, gsl_matrix_uint * dest, const gsl_matrix_uint * src);

GSL_FUN unsigned int gsl_matrix_uint_max (const gsl_matrix_uint * m);
GSL_FUN unsigned int gsl_matrix_uint_min (const gsl_matrix_uint * m);
GSL_FUN void gsl_matrix_uint_minmax (const gsl_matrix_uint * m, unsigned int * min_out, unsigned int * max_out);

GSL_FUN void gsl_matrix_uint_max_index (const gsl_matrix_uint * m, size_t * imax, size_t *jmax);
GSL_FUN void gsl_matrix_uint_min_index (const gsl_matrix_uint * m, size_t * imin, size_t *jmin);
GSL_FUN void gsl_matrix_uint_minmax_index (const gsl_matrix_uint * m, size_t * imin, size_t * jmin, size_t * imax, size_t * jmax);

GSL_FUN int gsl_matrix_uint_equal (const gsl_matrix_uint * a, const gsl_matrix_uint * b);

GSL_FUN int gsl_matrix_uint_isnull (const gsl_matrix_uint * m);
GSL_FUN int gsl_matrix_uint_ispos (const gsl_matrix_uint * m);
GSL_FUN int gsl_matrix_uint_isneg (const gsl_matrix_uint * m);
GSL_FUN int gsl_matrix_uint_isnonneg (const gsl_matrix_uint * m);

GSL_FUN int gsl_matrix_uint_add (gsl_matrix_uint * a, const gsl_matrix_uint * b);
GSL_FUN int gsl_matrix_uint_sub (gsl_matrix_uint * a, const gsl_matrix_uint * b);
GSL_FUN int gsl_matrix_uint_mul_elements (gsl_matrix_uint * a, const gsl_matrix_uint * b);
GSL_FUN int gsl_matrix_uint_div_elements (gsl_matrix_uint * a, const gsl_matrix_uint * b);
GSL_FUN int gsl_matrix_uint_scale (gsl_matrix_uint * a, const double x);
GSL_FUN int gsl_matrix_uint_add_constant (gsl_matrix_uint * a, const double x);
GSL_FUN int gsl_matrix_uint_add_diagonal (gsl_matrix_uint * a, const double x);

/***********************************************************************/
/* The functions below are obsolete                                    */
/***********************************************************************/
GSL_FUN int gsl_matrix_uint_get_row(gsl_vector_uint * v, const gsl_matrix_uint * m, const size_t i);
GSL_FUN int gsl_matrix_uint_get_col(gsl_vector_uint * v, const gsl_matrix_uint * m, const size_t j);
GSL_FUN int gsl_matrix_uint_set_row(gsl_matrix_uint * m, const size_t i, const gsl_vector_uint * v);
GSL_FUN int gsl_matrix_uint_set_col(gsl_matrix_uint * m, const size_t j, const gsl_vector_uint * v);
/***********************************************************************/

/* inline functions if you are using GCC */

GSL_FUN INLINE_DECL unsigned int   gsl_matrix_uint_get(const gsl_matrix_uint * m, const size_t i, const size_t j);
GSL_FUN INLINE_DECL void    gsl_matrix_uint_set(gsl_matrix_uint * m, const size_t i, const size_t j, const unsigned int x);
GSL_FUN INLINE_DECL unsigned int * gsl_matrix_uint_ptr(gsl_matrix_uint * m, const size_t i, const size_t j);
GSL_FUN INLINE_DECL const unsigned int * gsl_matrix_uint_const_ptr(const gsl_matrix_uint * m, const size_t i, const size_t j);

#ifdef HAVE_INLINE
INLINE_FUN 
unsigned int
gsl_matrix_uint_get(const gsl_matrix_uint * m, const size_t i, const size_t j)
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
gsl_matrix_uint_set(gsl_matrix_uint * m, const size_t i, const size_t j, const unsigned int x)
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
unsigned int *
gsl_matrix_uint_ptr(gsl_matrix_uint * m, const size_t i, const size_t j)
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
  return (unsigned int *) (m->data + (i * m->tda + j)) ;
} 

INLINE_FUN 
const unsigned int *
gsl_matrix_uint_const_ptr(const gsl_matrix_uint * m, const size_t i, const size_t j)
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
  return (const unsigned int *) (m->data + (i * m->tda + j)) ;
} 

#endif

__END_DECLS

#endif /* __GSL_MATRIX_UINT_H__ */
