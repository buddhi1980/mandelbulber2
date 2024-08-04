/* sys/gsl_sys.h
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

#ifndef __GSL_SYS_H__
#define __GSL_SYS_H__

#if !defined( GSL_FUN )
#  if !defined( GSL_DLL )
#    define GSL_FUN extern
#  elif defined( BUILD_GSL_DLL )
#    define GSL_FUN extern __declspec(dllexport)
#  else
#    define GSL_FUN extern __declspec(dllimport)
#  endif
#endif

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

GSL_FUN double gsl_log1p (const double x);
GSL_FUN double gsl_expm1 (const double x);
GSL_FUN double gsl_hypot (const double x, const double y);
GSL_FUN double gsl_hypot3 (const double x, const double y, const double z);
GSL_FUN double gsl_acosh (const double x);
GSL_FUN double gsl_asinh (const double x);
GSL_FUN double gsl_atanh (const double x);

GSL_FUN int gsl_isnan (const double x);
GSL_FUN int gsl_isinf (const double x);
GSL_FUN int gsl_finite (const double x);

GSL_FUN double gsl_nan (void);
GSL_FUN double gsl_posinf (void);
GSL_FUN double gsl_neginf (void);
GSL_FUN double gsl_fdiv (const double x, const double y);

GSL_FUN double gsl_coerce_double (const double x);
GSL_FUN float gsl_coerce_float (const float x);
GSL_FUN long double gsl_coerce_long_double (const long double x);

GSL_FUN double gsl_ldexp(const double x, const int e);
GSL_FUN double gsl_frexp(const double x, int * e);

GSL_FUN int gsl_fcmp (const double x1, const double x2, const double epsilon);

__END_DECLS

#endif /* __GSL_SYS_H__ */
