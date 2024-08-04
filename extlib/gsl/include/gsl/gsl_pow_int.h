/* gsl_pow_int.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2004, 2007 Gerard Jungman, Brian Gough
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

#ifndef __GSL_POW_INT_H__
#define __GSL_POW_INT_H__

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

GSL_FUN INLINE_DECL double gsl_pow_2(const double x);
GSL_FUN INLINE_DECL double gsl_pow_3(const double x);
GSL_FUN INLINE_DECL double gsl_pow_4(const double x);
GSL_FUN INLINE_DECL double gsl_pow_5(const double x);
GSL_FUN INLINE_DECL double gsl_pow_6(const double x);
GSL_FUN INLINE_DECL double gsl_pow_7(const double x);
GSL_FUN INLINE_DECL double gsl_pow_8(const double x);
GSL_FUN INLINE_DECL double gsl_pow_9(const double x);

#ifdef HAVE_INLINE
INLINE_FUN double gsl_pow_2(const double x) { return x*x;   }
INLINE_FUN double gsl_pow_3(const double x) { return x*x*x; }
INLINE_FUN double gsl_pow_4(const double x) { double x2 = x*x;   return x2*x2;    }
INLINE_FUN double gsl_pow_5(const double x) { double x2 = x*x;   return x2*x2*x;  }
INLINE_FUN double gsl_pow_6(const double x) { double x2 = x*x;   return x2*x2*x2; }
INLINE_FUN double gsl_pow_7(const double x) { double x3 = x*x*x; return x3*x3*x;  }
INLINE_FUN double gsl_pow_8(const double x) { double x2 = x*x;   double x4 = x2*x2; return x4*x4; }
INLINE_FUN double gsl_pow_9(const double x) { double x3 = x*x*x; return x3*x3*x3; }
#endif

GSL_FUN double gsl_pow_int(double x, int n);
GSL_FUN double gsl_pow_uint(double x, unsigned int n);

__END_DECLS

#endif /* __GSL_POW_INT_H__ */
