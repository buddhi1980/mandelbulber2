/* gsl_sf_hermite.h
 * 
 * Copyright (C) 2011-2014 Konrad Griessinger
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

/*----------------------------------------------------------------------*
 * (konradg(at)gmx.net)                                                 *
 *----------------------------------------------------------------------*/

#ifndef __GSL_SF_HERMITE_H__
#define __GSL_SF_HERMITE_H__

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

GSL_FUN int gsl_sf_hermite_prob_e(const int n, const double x, gsl_sf_result * result);
GSL_FUN double gsl_sf_hermite_prob(const int n, const double x);
GSL_FUN int gsl_sf_hermite_prob_der_e(const int m, const int n, const double x, gsl_sf_result * result);
GSL_FUN double gsl_sf_hermite_prob_der(const int m, const int n, const double x);
GSL_FUN int gsl_sf_hermite_phys_e(const int n, const double x, gsl_sf_result * result);
GSL_FUN double gsl_sf_hermite_phys(const int n, const double x);
GSL_FUN int gsl_sf_hermite_phys_der_e(const int m, const int n, const double x, gsl_sf_result * result);
GSL_FUN double gsl_sf_hermite_phys_der(const int m, const int n, const double x);
GSL_FUN int gsl_sf_hermite_func_e(const int n, const double x, gsl_sf_result * result);
GSL_FUN double gsl_sf_hermite_func(const int n, const double x);
GSL_FUN int gsl_sf_hermite_prob_array(const int nmax, const double x, double * result_array);
GSL_FUN int gsl_sf_hermite_prob_array_der(const int m, const int nmax, const double x, double * result_array);
GSL_FUN int gsl_sf_hermite_prob_der_array(const int mmax, const int n, const double x, double * result_array);
GSL_FUN int gsl_sf_hermite_prob_series_e(const int n, const double x, const double * a, gsl_sf_result * result);
GSL_FUN double gsl_sf_hermite_prob_series(const int n, const double x, const double * a);
GSL_FUN int gsl_sf_hermite_phys_array(const int nmax, const double x, double * result_array);
GSL_FUN int gsl_sf_hermite_phys_array_der(const int m, const int nmax, const double x, double * result_array);
GSL_FUN int gsl_sf_hermite_phys_der_array(const int mmax, const int n, const double x, double * result_array);
GSL_FUN int gsl_sf_hermite_phys_series_e(const int n, const double x, const double * a, gsl_sf_result * result);
GSL_FUN double gsl_sf_hermite_phys_series(const int n, const double x, const double * a);
GSL_FUN int gsl_sf_hermite_func_array(const int nmax, const double x, double * result_array);
GSL_FUN int gsl_sf_hermite_func_series_e(const int n, const double x, const double * a, gsl_sf_result * result);
GSL_FUN double gsl_sf_hermite_func_series(const int n, const double x, const double * a);
GSL_FUN int gsl_sf_hermite_func_der_e(const int m, const int n, const double x, gsl_sf_result * result);
GSL_FUN double gsl_sf_hermite_func_der(const int m, const int n, const double x);
GSL_FUN int gsl_sf_hermite_prob_zero_e(const int n, const int s, gsl_sf_result * result);
GSL_FUN double gsl_sf_hermite_prob_zero(const int n, const int s);
GSL_FUN int gsl_sf_hermite_phys_zero_e(const int n, const int s, gsl_sf_result * result);
GSL_FUN double gsl_sf_hermite_phys_zero(const int n, const int s);
GSL_FUN int gsl_sf_hermite_func_zero_e(const int n, const int s, gsl_sf_result * result);
GSL_FUN double gsl_sf_hermite_func_zero(const int n, const int s);

__END_DECLS

#endif /* __GSL_SF_HERMITE_H__ */
