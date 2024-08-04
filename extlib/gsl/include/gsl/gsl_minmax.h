/* gsl_minmax.h
 * 
 * Copyright (C) 2008 Gerard Jungman, Brian Gough
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

#ifndef __GSL_MINMAX_H__
#define __GSL_MINMAX_H__

#if !defined( GSL_FUN )
#  if !defined( GSL_DLL )
#    define GSL_FUN extern
#  elif defined( BUILD_GSL_DLL )
#    define GSL_FUN extern __declspec(dllexport)
#  else
#    define GSL_FUN extern __declspec(dllimport)
#  endif
#endif
#include <gsl/gsl_inline.h>

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

/* Define MAX and MIN macros/functions if they don't exist. */

/* plain old macros for general use */
#define GSL_MAX(a,b) ((a) > (b) ? (a) : (b))
#define GSL_MIN(a,b) ((a) < (b) ? (a) : (b))

/* function versions of the above, in case they are needed */
GSL_FUN double gsl_max (double a, double b);
GSL_FUN double gsl_min (double a, double b);

/* inline-friendly strongly typed versions */
#ifdef HAVE_INLINE

INLINE_FUN int GSL_MAX_INT (int a, int b);
INLINE_FUN int GSL_MIN_INT (int a, int b);
INLINE_FUN double GSL_MAX_DBL (double a, double b);
INLINE_FUN double GSL_MIN_DBL (double a, double b);
INLINE_FUN long double GSL_MAX_LDBL (long double a, long double b);
INLINE_FUN long double GSL_MIN_LDBL (long double a, long double b);

INLINE_FUN int
GSL_MAX_INT (int a, int b)
{
  return GSL_MAX (a, b);
}

INLINE_FUN int
GSL_MIN_INT (int a, int b)
{
  return GSL_MIN (a, b);
}

INLINE_FUN double
GSL_MAX_DBL (double a, double b)
{
  return GSL_MAX (a, b);
}

INLINE_FUN double
GSL_MIN_DBL (double a, double b)
{
  return GSL_MIN (a, b);
}

INLINE_FUN long double
GSL_MAX_LDBL (long double a, long double b)
{
  return GSL_MAX (a, b);
}

INLINE_FUN long double
GSL_MIN_LDBL (long double a, long double b)
{
  return GSL_MIN (a, b);
}
#else
#define GSL_MAX_INT(a,b)   GSL_MAX(a,b)
#define GSL_MIN_INT(a,b)   GSL_MIN(a,b)
#define GSL_MAX_DBL(a,b)   GSL_MAX(a,b)
#define GSL_MIN_DBL(a,b)   GSL_MIN(a,b)
#define GSL_MAX_LDBL(a,b)  GSL_MAX(a,b)
#define GSL_MIN_LDBL(a,b)  GSL_MIN(a,b)
#endif /* HAVE_INLINE */

__END_DECLS

#endif /* __GSL_POW_INT_H__ */
