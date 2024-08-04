/* monte/gsl_monte_miser.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Michael Booth
 * Copyright (C) 2009 Brian Gough
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

/* Author: MJB */

#ifndef __GSL_MONTE_MISER_H__
#define __GSL_MONTE_MISER_H__

#if !defined( GSL_FUN )
#  if !defined( GSL_DLL )
#    define GSL_FUN extern
#  elif defined( BUILD_GSL_DLL )
#    define GSL_FUN extern __declspec(dllexport)
#  else
#    define GSL_FUN extern __declspec(dllimport)
#  endif
#endif

#include <gsl/gsl_rng.h>
#include <gsl/gsl_monte.h>
#include <gsl/gsl_monte_plain.h>

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

typedef struct {
  size_t min_calls;
  size_t min_calls_per_bisection;
  double dither;
  double estimate_frac;
  double alpha;
  size_t dim;
  int estimate_style;
  int depth;
  int verbose;
  double * x;
  double * xmid;
  double * sigma_l;
  double * sigma_r;
  double * fmax_l;
  double * fmax_r;
  double * fmin_l;
  double * fmin_r;
  double * fsum_l;
  double * fsum_r;
  double * fsum2_l;
  double * fsum2_r;
  size_t * hits_l;
  size_t * hits_r;
} gsl_monte_miser_state; 

GSL_FUN int gsl_monte_miser_integrate(gsl_monte_function * f, 
                              const double xl[], const double xh[], 
                              size_t dim, size_t calls, 
                              gsl_rng *r, 
                              gsl_monte_miser_state* state,
                              double *result, double *abserr);

GSL_FUN gsl_monte_miser_state* gsl_monte_miser_alloc(size_t dim);

GSL_FUN int gsl_monte_miser_init(gsl_monte_miser_state* state);

GSL_FUN void gsl_monte_miser_free(gsl_monte_miser_state* state);

typedef struct {
  double estimate_frac;
  size_t min_calls;
  size_t min_calls_per_bisection;
  double alpha;
  double dither;
} gsl_monte_miser_params;

GSL_FUN void gsl_monte_miser_params_get (const gsl_monte_miser_state * state,
				 gsl_monte_miser_params * params);

GSL_FUN void gsl_monte_miser_params_set (gsl_monte_miser_state * state,
				 const gsl_monte_miser_params * params);

__END_DECLS

#endif /* __GSL_MONTE_MISER_H__ */
