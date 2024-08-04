/* specfunc/gsl_sf_sincos_pi.h
 * 
 * Copyright (C) 2017 Konrad Griessinger
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

/* Author:  G. Jungman, K. Griessinger */

#ifndef __GSL_SF_SINCOS_PI_H__
#define __GSL_SF_SINCOS_PI_H__

#if !defined( GSL_FUN )
#  if !defined( GSL_DLL )
#    define GSL_FUN extern
#  elif defined( BUILD_GSL_DLL )
#    define GSL_FUN extern __declspec(dllexport)
#  else
#    define GSL_FUN extern __declspec(dllimport)
#  endif
#endif

#include <gsl/gsl_sf_result.h>

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


/* sin_pi(x) with GSL semantics. This is actually important
 * because we want to control the error estimate, and trying
 * to guess the error for the standard library implementation
 * every time it is used would be a little goofy.
 */
GSL_FUN int gsl_sf_sin_pi_e(double x, gsl_sf_result * result);
GSL_FUN double gsl_sf_sin_pi(const double x);


/* cos_pi(x) with GSL semantics.
 */
GSL_FUN int gsl_sf_cos_pi_e(double x, gsl_sf_result * result);
GSL_FUN double gsl_sf_cos_pi(const double x);


__END_DECLS

#endif /* __GSL_SF_SINCOS_PI_H__ */
