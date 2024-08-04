/* monte/gsl_monte_vegas.h
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

/* header for the gsl "vegas" routines.  Mike Booth, May 1998 */

#ifndef __GSL_MONTE_VEGAS_H__
#define __GSL_MONTE_VEGAS_H__

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
#include <gsl/gsl_rng.h>
#include <gsl/gsl_monte.h>

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

enum {GSL_VEGAS_MODE_IMPORTANCE = 1, 
      GSL_VEGAS_MODE_IMPORTANCE_ONLY = 0, 
      GSL_VEGAS_MODE_STRATIFIED = -1};

typedef struct {
  /* grid */
  size_t dim;
  size_t bins_max;
  unsigned int bins;
  unsigned int boxes; /* these are both counted along the axes */
  double * xi;
  double * xin;
  double * delx;
  double * weight;
  double vol;

  double * x;
  int * bin;
  int * box;
  
  /* distribution */
  double * d;

  /* control variables */
  double alpha;
  int mode;
  int verbose;
  unsigned int iterations;
  int stage;

  /* scratch variables preserved between calls to vegas1/2/3  */
  double jac;
  double wtd_int_sum; 
  double sum_wgts;
  double chi_sum;
  double chisq;

  double result;
  double sigma;

  unsigned int it_start;
  unsigned int it_num;
  unsigned int samples;
  unsigned int calls_per_box;

  FILE * ostream;

} gsl_monte_vegas_state;

GSL_FUN int gsl_monte_vegas_integrate(gsl_monte_function * f, 
                              double xl[], double xu[], 
                              size_t dim, size_t calls,
                              gsl_rng * r,
                              gsl_monte_vegas_state *state,
                              double* result, double* abserr);

GSL_FUN gsl_monte_vegas_state* gsl_monte_vegas_alloc(size_t dim);

GSL_FUN int gsl_monte_vegas_init(gsl_monte_vegas_state* state);

GSL_FUN void gsl_monte_vegas_free (gsl_monte_vegas_state* state);

GSL_FUN double gsl_monte_vegas_chisq (const gsl_monte_vegas_state* state);
GSL_FUN void gsl_monte_vegas_runval (const gsl_monte_vegas_state* state, double * result, double * sigma);

typedef struct {
  double alpha;
  size_t iterations;
  int stage;
  int mode;
  int verbose;
  FILE * ostream;
} gsl_monte_vegas_params;

GSL_FUN void gsl_monte_vegas_params_get (const gsl_monte_vegas_state * state,
				 gsl_monte_vegas_params * params);

GSL_FUN void gsl_monte_vegas_params_set (gsl_monte_vegas_state * state,
				 const gsl_monte_vegas_params * params);

__END_DECLS

#endif /* __GSL_MONTE_VEGAS_H__ */

