/* vector/gsl_vector_char.h
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

#ifndef __GSL_VECTOR_CHAR_H__
#define __GSL_VECTOR_CHAR_H__

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
#include <gsl/gsl_block_char.h>

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
  char *data;
  gsl_block_char *block;
  int owner;
} 
gsl_vector_char;

typedef struct
{
  gsl_vector_char vector;
} _gsl_vector_char_view;

typedef _gsl_vector_char_view gsl_vector_char_view;

typedef struct
{
  gsl_vector_char vector;
} _gsl_vector_char_const_view;

typedef const _gsl_vector_char_const_view gsl_vector_char_const_view;


/* Allocation */

GSL_FUN gsl_vector_char *gsl_vector_char_alloc (const size_t n);
GSL_FUN gsl_vector_char *gsl_vector_char_calloc (const size_t n);

GSL_FUN gsl_vector_char *gsl_vector_char_alloc_from_block (gsl_block_char * b,
                                                     const size_t offset, 
                                                     const size_t n, 
                                                     const size_t stride);

GSL_FUN gsl_vector_char *gsl_vector_char_alloc_from_vector (gsl_vector_char * v,
                                                      const size_t offset, 
                                                      const size_t n, 
                                                      const size_t stride);

GSL_FUN void gsl_vector_char_free (gsl_vector_char * v);

/* Views */

GSL_FUN _gsl_vector_char_view 
gsl_vector_char_view_array (char *v, size_t n);

GSL_FUN _gsl_vector_char_view 
gsl_vector_char_view_array_with_stride (char *base,
                                         size_t stride,
                                         size_t n);

GSL_FUN _gsl_vector_char_const_view 
gsl_vector_char_const_view_array (const char *v, size_t n);

GSL_FUN _gsl_vector_char_const_view 
gsl_vector_char_const_view_array_with_stride (const char *base,
                                               size_t stride,
                                               size_t n);

GSL_FUN _gsl_vector_char_view 
gsl_vector_char_subvector (gsl_vector_char *v, 
                            size_t i, 
                            size_t n);

GSL_FUN _gsl_vector_char_view 
gsl_vector_char_subvector_with_stride (gsl_vector_char *v, 
                                        size_t i,
                                        size_t stride,
                                        size_t n);

GSL_FUN _gsl_vector_char_const_view 
gsl_vector_char_const_subvector (const gsl_vector_char *v, 
                                  size_t i, 
                                  size_t n);

GSL_FUN _gsl_vector_char_const_view 
gsl_vector_char_const_subvector_with_stride (const gsl_vector_char *v, 
                                              size_t i, 
                                              size_t stride,
                                              size_t n);

/* Operations */

GSL_FUN void gsl_vector_char_set_zero (gsl_vector_char * v);
GSL_FUN void gsl_vector_char_set_all (gsl_vector_char * v, char x);
GSL_FUN int gsl_vector_char_set_basis (gsl_vector_char * v, size_t i);

GSL_FUN int gsl_vector_char_fread (FILE * stream, gsl_vector_char * v);
GSL_FUN int gsl_vector_char_fwrite (FILE * stream, const gsl_vector_char * v);
GSL_FUN int gsl_vector_char_fscanf (FILE * stream, gsl_vector_char * v);
GSL_FUN int gsl_vector_char_fprintf (FILE * stream, const gsl_vector_char * v,
                              const char *format);

GSL_FUN int gsl_vector_char_memcpy (gsl_vector_char * dest, const gsl_vector_char * src);

GSL_FUN int gsl_vector_char_reverse (gsl_vector_char * v);

GSL_FUN int gsl_vector_char_swap (gsl_vector_char * v, gsl_vector_char * w);
GSL_FUN int gsl_vector_char_swap_elements (gsl_vector_char * v, const size_t i, const size_t j);

GSL_FUN char gsl_vector_char_max (const gsl_vector_char * v);
GSL_FUN char gsl_vector_char_min (const gsl_vector_char * v);
GSL_FUN void gsl_vector_char_minmax (const gsl_vector_char * v, char * min_out, char * max_out);

GSL_FUN size_t gsl_vector_char_max_index (const gsl_vector_char * v);
GSL_FUN size_t gsl_vector_char_min_index (const gsl_vector_char * v);
GSL_FUN void gsl_vector_char_minmax_index (const gsl_vector_char * v, size_t * imin, size_t * imax);

GSL_FUN int gsl_vector_char_add (gsl_vector_char * a, const gsl_vector_char * b);
GSL_FUN int gsl_vector_char_sub (gsl_vector_char * a, const gsl_vector_char * b);
GSL_FUN int gsl_vector_char_mul (gsl_vector_char * a, const gsl_vector_char * b);
GSL_FUN int gsl_vector_char_div (gsl_vector_char * a, const gsl_vector_char * b);
GSL_FUN int gsl_vector_char_scale (gsl_vector_char * a, const double x);
GSL_FUN int gsl_vector_char_add_constant (gsl_vector_char * a, const double x);

GSL_FUN int gsl_vector_char_equal (const gsl_vector_char * u, 
                            const gsl_vector_char * v);

GSL_FUN int gsl_vector_char_isnull (const gsl_vector_char * v);
GSL_FUN int gsl_vector_char_ispos (const gsl_vector_char * v);
GSL_FUN int gsl_vector_char_isneg (const gsl_vector_char * v);
GSL_FUN int gsl_vector_char_isnonneg (const gsl_vector_char * v);

GSL_FUN INLINE_DECL char gsl_vector_char_get (const gsl_vector_char * v, const size_t i);
GSL_FUN INLINE_DECL void gsl_vector_char_set (gsl_vector_char * v, const size_t i, char x);
GSL_FUN INLINE_DECL char * gsl_vector_char_ptr (gsl_vector_char * v, const size_t i);
GSL_FUN INLINE_DECL const char * gsl_vector_char_const_ptr (const gsl_vector_char * v, const size_t i);

#ifdef HAVE_INLINE

INLINE_FUN
char
gsl_vector_char_get (const gsl_vector_char * v, const size_t i)
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
gsl_vector_char_set (gsl_vector_char * v, const size_t i, char x)
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
char *
gsl_vector_char_ptr (gsl_vector_char * v, const size_t i)
{
#if GSL_RANGE_CHECK
  if (GSL_RANGE_COND(i >= v->size))
    {
      GSL_ERROR_NULL ("index out of range", GSL_EINVAL);
    }
#endif
  return (char *) (v->data + i * v->stride);
}

INLINE_FUN
const char *
gsl_vector_char_const_ptr (const gsl_vector_char * v, const size_t i)
{
#if GSL_RANGE_CHECK
  if (GSL_RANGE_COND(i >= v->size))
    {
      GSL_ERROR_NULL ("index out of range", GSL_EINVAL);
    }
#endif
  return (const char *) (v->data + i * v->stride);
}
#endif /* HAVE_INLINE */

__END_DECLS

#endif /* __GSL_VECTOR_CHAR_H__ */


