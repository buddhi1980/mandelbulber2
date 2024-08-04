/* matrix/gsl_matrix_complex_double.h
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

#ifndef __GSL_MATRIX_COMPLEX_DOUBLE_H__
#define __GSL_MATRIX_COMPLEX_DOUBLE_H__

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
#include <gsl/gsl_complex.h>
#include <gsl/gsl_check_range.h>
#include <gsl/gsl_vector_complex_double.h>

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
  gsl_block_complex * block;
  int owner;
} gsl_matrix_complex ;

typedef struct
{
  gsl_matrix_complex matrix;
} _gsl_matrix_complex_view;

typedef _gsl_matrix_complex_view gsl_matrix_complex_view;

typedef struct
{
  gsl_matrix_complex matrix;
} _gsl_matrix_complex_const_view;

typedef const _gsl_matrix_complex_const_view gsl_matrix_complex_const_view;


/* Allocation */

GSL_FUN gsl_matrix_complex * 
gsl_matrix_complex_alloc (const size_t n1, const size_t n2);

GSL_FUN gsl_matrix_complex * 
gsl_matrix_complex_calloc (const size_t n1, const size_t n2);

GSL_FUN gsl_matrix_complex * 
gsl_matrix_complex_alloc_from_block (gsl_block_complex * b, 
                                           const size_t offset, 
                                           const size_t n1, const size_t n2, const size_t d2);

GSL_FUN gsl_matrix_complex * 
gsl_matrix_complex_alloc_from_matrix (gsl_matrix_complex * b,
                                            const size_t k1, const size_t k2,
                                            const size_t n1, const size_t n2);

GSL_FUN gsl_vector_complex * 
gsl_vector_complex_alloc_row_from_matrix (gsl_matrix_complex * m,
                                                const size_t i);

GSL_FUN gsl_vector_complex * 
gsl_vector_complex_alloc_col_from_matrix (gsl_matrix_complex * m,
                                                const size_t j);

GSL_FUN void gsl_matrix_complex_free (gsl_matrix_complex * m);

/* Views */

GSL_FUN _gsl_matrix_complex_view 
gsl_matrix_complex_submatrix (gsl_matrix_complex * m, 
                            const size_t i, const size_t j, 
                            const size_t n1, const size_t n2);

GSL_FUN _gsl_vector_complex_view 
gsl_matrix_complex_row (gsl_matrix_complex * m, const size_t i);

GSL_FUN _gsl_vector_complex_view 
gsl_matrix_complex_column (gsl_matrix_complex * m, const size_t j);

GSL_FUN _gsl_vector_complex_view 
gsl_matrix_complex_diagonal (gsl_matrix_complex * m);

GSL_FUN _gsl_vector_complex_view 
gsl_matrix_complex_subdiagonal (gsl_matrix_complex * m, const size_t k);

GSL_FUN _gsl_vector_complex_view 
gsl_matrix_complex_superdiagonal (gsl_matrix_complex * m, const size_t k);

GSL_FUN _gsl_vector_complex_view
gsl_matrix_complex_subrow (gsl_matrix_complex * m,
                                 const size_t i, const size_t offset,
                                 const size_t n);

GSL_FUN _gsl_vector_complex_view
gsl_matrix_complex_subcolumn (gsl_matrix_complex * m,
                                    const size_t j, const size_t offset,
                                    const size_t n);

GSL_FUN _gsl_matrix_complex_view
gsl_matrix_complex_view_array (double * base,
                             const size_t n1, 
                             const size_t n2);

GSL_FUN _gsl_matrix_complex_view
gsl_matrix_complex_view_array_with_tda (double * base, 
                                      const size_t n1, 
                                      const size_t n2,
                                      const size_t tda);

GSL_FUN _gsl_matrix_complex_view
gsl_matrix_complex_view_vector (gsl_vector_complex * v,
                              const size_t n1, 
                              const size_t n2);

GSL_FUN _gsl_matrix_complex_view
gsl_matrix_complex_view_vector_with_tda (gsl_vector_complex * v,
                                       const size_t n1, 
                                       const size_t n2,
                                       const size_t tda);


GSL_FUN _gsl_matrix_complex_const_view 
gsl_matrix_complex_const_submatrix (const gsl_matrix_complex * m, 
                                  const size_t i, const size_t j, 
                                  const size_t n1, const size_t n2);

GSL_FUN _gsl_vector_complex_const_view 
gsl_matrix_complex_const_row (const gsl_matrix_complex * m, 
                            const size_t i);

GSL_FUN _gsl_vector_complex_const_view 
gsl_matrix_complex_const_column (const gsl_matrix_complex * m, 
                               const size_t j);

GSL_FUN _gsl_vector_complex_const_view
gsl_matrix_complex_const_diagonal (const gsl_matrix_complex * m);

GSL_FUN _gsl_vector_complex_const_view 
gsl_matrix_complex_const_subdiagonal (const gsl_matrix_complex * m, 
                                    const size_t k);

GSL_FUN _gsl_vector_complex_const_view 
gsl_matrix_complex_const_superdiagonal (const gsl_matrix_complex * m, 
                                      const size_t k);

GSL_FUN _gsl_vector_complex_const_view
gsl_matrix_complex_const_subrow (const gsl_matrix_complex * m,
                                       const size_t i, const size_t offset,
                                       const size_t n);

GSL_FUN _gsl_vector_complex_const_view
gsl_matrix_complex_const_subcolumn (const gsl_matrix_complex * m,
                                          const size_t j, const size_t offset,
                                          const size_t n);

GSL_FUN _gsl_matrix_complex_const_view
gsl_matrix_complex_const_view_array (const double * base,
                                   const size_t n1, 
                                   const size_t n2);

GSL_FUN _gsl_matrix_complex_const_view
gsl_matrix_complex_const_view_array_with_tda (const double * base, 
                                            const size_t n1, 
                                            const size_t n2,
                                            const size_t tda);

GSL_FUN _gsl_matrix_complex_const_view
gsl_matrix_complex_const_view_vector (const gsl_vector_complex * v,
                                    const size_t n1, 
                                    const size_t n2);

GSL_FUN _gsl_matrix_complex_const_view
gsl_matrix_complex_const_view_vector_with_tda (const gsl_vector_complex * v,
                                             const size_t n1, 
                                             const size_t n2,
                                             const size_t tda);

/* Operations */

GSL_FUN void gsl_matrix_complex_set_zero (gsl_matrix_complex * m);
GSL_FUN void gsl_matrix_complex_set_identity (gsl_matrix_complex * m);
GSL_FUN void gsl_matrix_complex_set_all (gsl_matrix_complex * m, gsl_complex x);

GSL_FUN int gsl_matrix_complex_fread (FILE * stream, gsl_matrix_complex * m) ;
GSL_FUN int gsl_matrix_complex_fwrite (FILE * stream, const gsl_matrix_complex * m) ;
GSL_FUN int gsl_matrix_complex_fscanf (FILE * stream, gsl_matrix_complex * m);
GSL_FUN int gsl_matrix_complex_fprintf (FILE * stream, const gsl_matrix_complex * m, const char * format);

GSL_FUN int gsl_matrix_complex_memcpy(gsl_matrix_complex * dest, const gsl_matrix_complex * src);
GSL_FUN int gsl_matrix_complex_swap(gsl_matrix_complex * m1, gsl_matrix_complex * m2);
GSL_FUN int gsl_matrix_complex_tricpy(const char uplo_src, const int copy_diag, gsl_matrix_complex * dest, const gsl_matrix_complex * src);

GSL_FUN int gsl_matrix_complex_swap_rows(gsl_matrix_complex * m, const size_t i, const size_t j);
GSL_FUN int gsl_matrix_complex_swap_columns(gsl_matrix_complex * m, const size_t i, const size_t j);
GSL_FUN int gsl_matrix_complex_swap_rowcol(gsl_matrix_complex * m, const size_t i, const size_t j);

GSL_FUN int gsl_matrix_complex_transpose (gsl_matrix_complex * m);
GSL_FUN int gsl_matrix_complex_transpose_memcpy (gsl_matrix_complex * dest, const gsl_matrix_complex * src);
GSL_FUN int gsl_matrix_complex_transpose_tricpy(const char uplo_src, const int copy_diag, gsl_matrix_complex * dest, const gsl_matrix_complex * src);

GSL_FUN int gsl_matrix_complex_equal (const gsl_matrix_complex * a, const gsl_matrix_complex * b);

GSL_FUN int gsl_matrix_complex_isnull (const gsl_matrix_complex * m);
GSL_FUN int gsl_matrix_complex_ispos (const gsl_matrix_complex * m);
GSL_FUN int gsl_matrix_complex_isneg (const gsl_matrix_complex * m);
GSL_FUN int gsl_matrix_complex_isnonneg (const gsl_matrix_complex * m);

GSL_FUN int gsl_matrix_complex_add (gsl_matrix_complex * a, const gsl_matrix_complex * b);
GSL_FUN int gsl_matrix_complex_sub (gsl_matrix_complex * a, const gsl_matrix_complex * b);
GSL_FUN int gsl_matrix_complex_mul_elements (gsl_matrix_complex * a, const gsl_matrix_complex * b);
GSL_FUN int gsl_matrix_complex_div_elements (gsl_matrix_complex * a, const gsl_matrix_complex * b);
GSL_FUN int gsl_matrix_complex_scale (gsl_matrix_complex * a, const gsl_complex x);
GSL_FUN int gsl_matrix_complex_add_constant (gsl_matrix_complex * a, const gsl_complex x);
GSL_FUN int gsl_matrix_complex_add_diagonal (gsl_matrix_complex * a, const gsl_complex x);

/***********************************************************************/
/* The functions below are obsolete                                    */
/***********************************************************************/
GSL_FUN int gsl_matrix_complex_get_row(gsl_vector_complex * v, const gsl_matrix_complex * m, const size_t i);
GSL_FUN int gsl_matrix_complex_get_col(gsl_vector_complex * v, const gsl_matrix_complex * m, const size_t j);
GSL_FUN int gsl_matrix_complex_set_row(gsl_matrix_complex * m, const size_t i, const gsl_vector_complex * v);
GSL_FUN int gsl_matrix_complex_set_col(gsl_matrix_complex * m, const size_t j, const gsl_vector_complex * v);
/***********************************************************************/

/* inline functions if you are using GCC */

GSL_FUN INLINE_DECL gsl_complex gsl_matrix_complex_get(const gsl_matrix_complex * m, const size_t i, const size_t j);
GSL_FUN INLINE_DECL void gsl_matrix_complex_set(gsl_matrix_complex * m, const size_t i, const size_t j, const gsl_complex x);

GSL_FUN INLINE_DECL gsl_complex * gsl_matrix_complex_ptr(gsl_matrix_complex * m, const size_t i, const size_t j);
GSL_FUN INLINE_DECL const gsl_complex * gsl_matrix_complex_const_ptr(const gsl_matrix_complex * m, const size_t i, const size_t j);

#ifdef HAVE_INLINE

INLINE_FUN 
gsl_complex
gsl_matrix_complex_get(const gsl_matrix_complex * m, 
                     const size_t i, const size_t j)
{
#if GSL_RANGE_CHECK
  if (GSL_RANGE_COND(1)) 
    {
      gsl_complex zero = {{0,0}};

      if (i >= m->size1)
        {
          GSL_ERROR_VAL("first index out of range", GSL_EINVAL, zero) ;
        }
      else if (j >= m->size2)
        {
          GSL_ERROR_VAL("second index out of range", GSL_EINVAL, zero) ;
        }
    }
#endif
  return *(gsl_complex *)(m->data + 2*(i * m->tda + j)) ;
} 

INLINE_FUN 
void
gsl_matrix_complex_set(gsl_matrix_complex * m, 
                     const size_t i, const size_t j, const gsl_complex x)
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
  *(gsl_complex *)(m->data + 2*(i * m->tda + j)) = x ;
}

INLINE_FUN 
gsl_complex *
gsl_matrix_complex_ptr(gsl_matrix_complex * m, 
                             const size_t i, const size_t j)
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
  return (gsl_complex *)(m->data + 2*(i * m->tda + j)) ;
} 

INLINE_FUN 
const gsl_complex *
gsl_matrix_complex_const_ptr(const gsl_matrix_complex * m, 
                                   const size_t i, const size_t j)
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
  return (const gsl_complex *)(m->data + 2*(i * m->tda + j)) ;
} 

#endif /* HAVE_INLINE */

__END_DECLS

#endif /* __GSL_MATRIX_COMPLEX_DOUBLE_H__ */
