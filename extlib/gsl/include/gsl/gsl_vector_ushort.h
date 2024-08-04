/* vector/gsl_vector_ushort.h
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

#ifndef __GSL_VECTOR_USHORT_H__
#define __GSL_VECTOR_USHORT_H__

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
#include <gsl/gsl_block_ushort.h>

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
  size_t size;
  size_t stride;
  unsigned short *data;
  gsl_block_ushort *block;
  int owner;
} 
gsl_vector_ushort;

typedef struct
{
  gsl_vector_ushort vector;
} _gsl_vector_ushort_view;

typedef _gsl_vector_ushort_view gsl_vector_ushort_view;

typedef struct
{
  gsl_vector_ushort vector;
} _gsl_vector_ushort_const_view;

typedef const _gsl_vector_ushort_const_view gsl_vector_ushort_const_view;


/* Allocation */

GSL_FUN gsl_vector_ushort *gsl_vector_ushort_alloc (const size_t n);
GSL_FUN gsl_vector_ushort *gsl_vector_ushort_calloc (const size_t n);

GSL_FUN gsl_vector_ushort *gsl_vector_ushort_alloc_from_block (gsl_block_ushort * b,
                                                     const size_t offset, 
                                                     const size_t n, 
                                                     const size_t stride);

GSL_FUN gsl_vector_ushort *gsl_vector_ushort_alloc_from_vector (gsl_vector_ushort * v,
                                                      const size_t offset, 
                                                      const size_t n, 
                                                      const size_t stride);

GSL_FUN void gsl_vector_ushort_free (gsl_vector_ushort * v);

/* Views */

GSL_FUN _gsl_vector_ushort_view 
gsl_vector_ushort_view_array (unsigned short *v, size_t n);

GSL_FUN _gsl_vector_ushort_view 
gsl_vector_ushort_view_array_with_stride (unsigned short *base,
                                         size_t stride,
                                         size_t n);

GSL_FUN _gsl_vector_ushort_const_view 
gsl_vector_ushort_const_view_array (const unsigned short *v, size_t n);

GSL_FUN _gsl_vector_ushort_const_view 
gsl_vector_ushort_const_view_array_with_stride (const unsigned short *base,
                                               size_t stride,
                                               size_t n);

GSL_FUN _gsl_vector_ushort_view 
gsl_vector_ushort_subvector (gsl_vector_ushort *v, 
                            size_t i, 
                            size_t n);

GSL_FUN _gsl_vector_ushort_view 
gsl_vector_ushort_subvector_with_stride (gsl_vector_ushort *v, 
                                        size_t i,
                                        size_t stride,
                                        size_t n);

GSL_FUN _gsl_vector_ushort_const_view 
gsl_vector_ushort_const_subvector (const gsl_vector_ushort *v, 
                                  size_t i, 
                                  size_t n);

GSL_FUN _gsl_vector_ushort_const_view 
gsl_vector_ushort_const_subvector_with_stride (const gsl_vector_ushort *v, 
                                              size_t i, 
                                              size_t stride,
                                              size_t n);

/* Operations */

GSL_FUN void gsl_vector_ushort_set_zero (gsl_vector_ushort * v);
GSL_FUN void gsl_vector_ushort_set_all (gsl_vector_ushort * v, unsigned short x);
GSL_FUN int gsl_vector_ushort_set_basis (gsl_vector_ushort * v, size_t i);

GSL_FUN int gsl_vector_ushort_fread (FILE * stream, gsl_vector_ushort * v);
GSL_FUN int gsl_vector_ushort_fwrite (FILE * stream, const gsl_vector_ushort * v);
GSL_FUN int gsl_vector_ushort_fscanf (FILE * stream, gsl_vector_ushort * v);
GSL_FUN int gsl_vector_ushort_fprintf (FILE * stream, const gsl_vector_ushort * v,
                              const char *format);

GSL_FUN int gsl_vector_ushort_memcpy (gsl_vector_ushort * dest, const gsl_vector_ushort * src);

GSL_FUN int gsl_vector_ushort_reverse (gsl_vector_ushort * v);

GSL_FUN int gsl_vector_ushort_swap (gsl_vector_ushort * v, gsl_vector_ushort * w);
GSL_FUN int gsl_vector_ushort_swap_elements (gsl_vector_ushort * v, const size_t i, const size_t j);

GSL_FUN unsigned short gsl_vector_ushort_max (const gsl_vector_ushort * v);
GSL_FUN unsigned short gsl_vector_ushort_min (const gsl_vector_ushort * v);
GSL_FUN void gsl_vector_ushort_minmax (const gsl_vector_ushort * v, unsigned short * min_out, unsigned short * max_out);

GSL_FUN size_t gsl_vector_ushort_max_index (const gsl_vector_ushort * v);
GSL_FUN size_t gsl_vector_ushort_min_index (const gsl_vector_ushort * v);
GSL_FUN void gsl_vector_ushort_minmax_index (const gsl_vector_ushort * v, size_t * imin, size_t * imax);

GSL_FUN int gsl_vector_ushort_add (gsl_vector_ushort * a, const gsl_vector_ushort * b);
GSL_FUN int gsl_vector_ushort_sub (gsl_vector_ushort * a, const gsl_vector_ushort * b);
GSL_FUN int gsl_vector_ushort_mul (gsl_vector_ushort * a, const gsl_vector_ushort * b);
GSL_FUN int gsl_vector_ushort_div (gsl_vector_ushort * a, const gsl_vector_ushort * b);
GSL_FUN int gsl_vector_ushort_scale (gsl_vector_ushort * a, const double x);
GSL_FUN int gsl_vector_ushort_add_constant (gsl_vector_ushort * a, const double x);

GSL_FUN int gsl_vector_ushort_equal (const gsl_vector_ushort * u, 
                            const gsl_vector_ushort * v);

GSL_FUN int gsl_vector_ushort_isnull (const gsl_vector_ushort * v);
GSL_FUN int gsl_vector_ushort_ispos (const gsl_vector_ushort * v);
GSL_FUN int gsl_vector_ushort_isneg (const gsl_vector_ushort * v);
GSL_FUN int gsl_vector_ushort_isnonneg (const gsl_vector_ushort * v);

GSL_FUN INLINE_DECL unsigned short gsl_vector_ushort_get (const gsl_vector_ushort * v, const size_t i);
GSL_FUN INLINE_DECL void gsl_vector_ushort_set (gsl_vector_ushort * v, const size_t i, unsigned short x);
GSL_FUN INLINE_DECL unsigned short * gsl_vector_ushort_ptr (gsl_vector_ushort * v, const size_t i);
GSL_FUN INLINE_DECL const unsigned short * gsl_vector_ushort_const_ptr (const gsl_vector_ushort * v, const size_t i);

#ifdef HAVE_INLINE

INLINE_FUN
unsigned short
gsl_vector_ushort_get (const gsl_vector_ushort * v, const size_t i)
{
#if GSL_RANGE_CHECK
  if (GSL_RANGE_COND(i >= v->size))
    {
      GSL_ERROR_VAL ("index out of range", GSL_EINVAL, 0);
    }
#endif
  return v->data[i * v->stride];
}

INLINE_FUN
void
gsl_vector_ushort_set (gsl_vector_ushort * v, const size_t i, unsigned short x)
{
#if GSL_RANGE_CHECK
  if (GSL_RANGE_COND(i >= v->size))
    {
      GSL_ERROR_VOID ("index out of range", GSL_EINVAL);
    }
#endif
  v->data[i * v->stride] = x;
}

INLINE_FUN
unsigned short *
gsl_vector_ushort_ptr (gsl_vector_ushort * v, const size_t i)
{
#if GSL_RANGE_CHECK
  if (GSL_RANGE_COND(i >= v->size))
    {
      GSL_ERROR_NULL ("index out of range", GSL_EINVAL);
    }
#endif
  return (unsigned short *) (v->data + i * v->stride);
}

INLINE_FUN
const unsigned short *
gsl_vector_ushort_const_ptr (const gsl_vector_ushort * v, const size_t i)
{
#if GSL_RANGE_CHECK
  if (GSL_RANGE_COND(i >= v->size))
    {
      GSL_ERROR_NULL ("index out of range", GSL_EINVAL);
    }
#endif
  return (const unsigned short *) (v->data + i * v->stride);
}
#endif /* HAVE_INLINE */

__END_DECLS

#endif /* __GSL_VECTOR_USHORT_H__ */


