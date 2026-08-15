/* filter/gsl_filter.h
 * 
 * Copyright (C) 2018 Patrick Alken
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

#ifndef __GSL_FILTER_H__
#define __GSL_FILTER_H__

#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_movstat.h>

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

/* end point handling methods */
typedef enum
{
  GSL_FILTER_END_PADZERO = GSL_MOVSTAT_END_PADZERO,
  GSL_FILTER_END_PADVALUE = GSL_MOVSTAT_END_PADVALUE,
  GSL_FILTER_END_TRUNCATE = GSL_MOVSTAT_END_TRUNCATE
} gsl_filter_end_t;

/* robust scale estimates */
typedef enum
{
  GSL_FILTER_SCALE_MAD, /* median absolute deviation */
  GSL_FILTER_SCALE_IQR, /* interquartile range */
  GSL_FILTER_SCALE_SN,  /* S_n scale statistic */
  GSL_FILTER_SCALE_QN   /* Q_n scale statistic */
} gsl_filter_scale_t;

/* workspace for Gaussian filter */
typedef struct
{
  size_t K;        /* window size */
  double *kernel;  /* Gaussian kernel, size K */
  gsl_movstat_workspace *movstat_workspace_p;
} gsl_filter_gaussian_workspace;

gsl_filter_gaussian_workspace *gsl_filter_gaussian_alloc(const size_t K);
void gsl_filter_gaussian_free(gsl_filter_gaussian_workspace * w);
int gsl_filter_gaussian(const gsl_filter_end_t endtype, const double alpha, const size_t order, const gsl_vector * x,
                        gsl_vector * y, gsl_filter_gaussian_workspace * w);
int gsl_filter_gaussian_kernel(const double alpha, const size_t order, const int normalize, gsl_vector * kernel);

/* workspace for standard median filter */
typedef struct
{
  gsl_movstat_workspace *movstat_workspace_p;
} gsl_filter_median_workspace;

gsl_filter_median_workspace *gsl_filter_median_alloc(const size_t K);
void gsl_filter_median_free(gsl_filter_median_workspace * w);
int gsl_filter_median(const gsl_filter_end_t endtype, const gsl_vector * x, gsl_vector * y, gsl_filter_median_workspace * w);

/* workspace for recursive median filter */
typedef struct
{
  size_t H;                            /* window half-length (K / 2) */
  size_t K;                            /* window size */
  void *state;                         /* workspace for min/max accumulator */
  double *window;                      /* array holding first window */
  const gsl_movstat_accum * minmaxacc; /* minimum/maximum accumulator */
  gsl_movstat_workspace *movstat_workspace_p;
} gsl_filter_rmedian_workspace;

gsl_filter_rmedian_workspace *gsl_filter_rmedian_alloc(const size_t K);
void gsl_filter_rmedian_free(gsl_filter_rmedian_workspace * w);
int gsl_filter_rmedian(const gsl_filter_end_t, const gsl_vector * x, gsl_vector * y, gsl_filter_rmedian_workspace * w);

typedef struct
{
  gsl_movstat_workspace *movstat_workspace_p;
} gsl_filter_impulse_workspace;

gsl_filter_impulse_workspace *gsl_filter_impulse_alloc(const size_t K);
void gsl_filter_impulse_free(gsl_filter_impulse_workspace * w);
int gsl_filter_impulse(const gsl_filter_end_t endtype, const gsl_filter_scale_t scale_type, const double t,
                       const gsl_vector * x, gsl_vector * y, gsl_vector * xmedian, gsl_vector * xsigma, size_t * noutlier,
                       gsl_vector_int * ioutlier, gsl_filter_impulse_workspace * w);

__END_DECLS

#endif /* __GSL_FILTER_H__ */
