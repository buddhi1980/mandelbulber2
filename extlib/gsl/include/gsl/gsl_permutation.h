/* permutation/gsl_permutation.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2004, 2007 Brian Gough
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

#ifndef __GSL_PERMUTATION_H__
#define __GSL_PERMUTATION_H__

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

struct gsl_permutation_struct
{
  size_t size;
  size_t *data;
};

typedef struct gsl_permutation_struct gsl_permutation;

GSL_FUN gsl_permutation *gsl_permutation_alloc (const size_t n);
GSL_FUN gsl_permutation *gsl_permutation_calloc (const size_t n);
GSL_FUN void gsl_permutation_init (gsl_permutation * p);
GSL_FUN void gsl_permutation_free (gsl_permutation * p);
GSL_FUN int gsl_permutation_memcpy (gsl_permutation * dest, const gsl_permutation * src);

GSL_FUN int gsl_permutation_fread (FILE * stream, gsl_permutation * p);
GSL_FUN int gsl_permutation_fwrite (FILE * stream, const gsl_permutation * p);
GSL_FUN int gsl_permutation_fscanf (FILE * stream, gsl_permutation * p);
GSL_FUN int gsl_permutation_fprintf (FILE * stream, const gsl_permutation * p, const char *format);

GSL_FUN size_t gsl_permutation_size (const gsl_permutation * p);
GSL_FUN size_t * gsl_permutation_data (const gsl_permutation * p);

GSL_FUN int gsl_permutation_swap (gsl_permutation * p, const size_t i, const size_t j);

GSL_FUN int gsl_permutation_valid (const gsl_permutation * p);
GSL_FUN void gsl_permutation_reverse (gsl_permutation * p);
GSL_FUN int gsl_permutation_inverse (gsl_permutation * inv, const gsl_permutation * p);
GSL_FUN int gsl_permutation_next (gsl_permutation * p);
GSL_FUN int gsl_permutation_prev (gsl_permutation * p);
GSL_FUN int gsl_permutation_mul (gsl_permutation * p, const gsl_permutation * pa, const gsl_permutation * pb);

GSL_FUN int gsl_permutation_linear_to_canonical (gsl_permutation * q, const gsl_permutation * p);
GSL_FUN int gsl_permutation_canonical_to_linear (gsl_permutation * p, const gsl_permutation * q);

GSL_FUN size_t gsl_permutation_inversions (const gsl_permutation * p);
GSL_FUN size_t gsl_permutation_linear_cycles (const gsl_permutation * p);
GSL_FUN size_t gsl_permutation_canonical_cycles (const gsl_permutation * q);

GSL_FUN INLINE_DECL size_t gsl_permutation_get (const gsl_permutation * p, const size_t i);

#ifdef HAVE_INLINE

INLINE_FUN
size_t
gsl_permutation_get (const gsl_permutation * p, const size_t i)
{
#if GSL_RANGE_CHECK
  if (GSL_RANGE_COND(i >= p->size))
    {
      GSL_ERROR_VAL ("index out of range", GSL_EINVAL, 0);
    }
#endif
  return p->data[i];
}

#endif /* HAVE_INLINE */

__END_DECLS

#endif /* __GSL_PERMUTATION_H__ */
