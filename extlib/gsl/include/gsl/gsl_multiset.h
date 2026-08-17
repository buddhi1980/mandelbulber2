/* multiset/gsl_multiset.h
 * based on combination/gsl_combination.h by Szymon Jaroszewicz
 * based on permutation/gsl_permutation.h by Brian Gough
 *
 * Copyright (C) 2009 Rhys Ulerich
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

#ifndef __GSL_MULTISET_H__
#define __GSL_MULTISET_H__

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
#include <gsl/gsl_errno.h>
#include <gsl/gsl_types.h>
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

struct gsl_multiset_struct
{
  size_t n;
  size_t k;
  size_t *data;
};

typedef struct gsl_multiset_struct gsl_multiset;

GSL_FUN gsl_multiset *gsl_multiset_alloc (const size_t n, const size_t k);
GSL_FUN gsl_multiset *gsl_multiset_calloc (const size_t n, const size_t k);
GSL_FUN void gsl_multiset_init_first (gsl_multiset * c);
GSL_FUN void gsl_multiset_init_last (gsl_multiset * c);
GSL_FUN void gsl_multiset_free (gsl_multiset * c);
GSL_FUN int gsl_multiset_memcpy (gsl_multiset * dest, const gsl_multiset * src);

GSL_FUN int gsl_multiset_fread (FILE * stream, gsl_multiset * c);
GSL_FUN int gsl_multiset_fwrite (FILE * stream, const gsl_multiset * c);
GSL_FUN int gsl_multiset_fscanf (FILE * stream, gsl_multiset * c);
GSL_FUN int gsl_multiset_fprintf (FILE * stream, const gsl_multiset * c, const char *format);

GSL_FUN size_t gsl_multiset_n (const gsl_multiset * c);
GSL_FUN size_t gsl_multiset_k (const gsl_multiset * c);
GSL_FUN size_t * gsl_multiset_data (const gsl_multiset * c);

GSL_FUN int gsl_multiset_valid (gsl_multiset * c);
GSL_FUN int gsl_multiset_next (gsl_multiset * c);
GSL_FUN int gsl_multiset_prev (gsl_multiset * c);

GSL_FUN INLINE_DECL size_t gsl_multiset_get (const gsl_multiset * c, const size_t i);

#ifdef HAVE_INLINE

INLINE_FUN
size_t
gsl_multiset_get (const gsl_multiset * c, const size_t i)
{
#if GSL_RANGE_CHECK
  if (GSL_RANGE_COND(i >= c->k)) /* size_t is unsigned, can't be negative */
    {
      GSL_ERROR_VAL ("index out of range", GSL_EINVAL, 0);
    }
#endif
  return c->data[i];
}

#endif /* HAVE_INLINE */

__END_DECLS

#endif /* __GSL_MULTISET_H__ */
