/* movstat/gsl_movstat.h
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

#ifndef __GSL_MOVSTAT_H__
#define __GSL_MOVSTAT_H__

#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>

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

typedef enum
{
  GSL_MOVSTAT_END_PADZERO,
  GSL_MOVSTAT_END_PADVALUE,
  GSL_MOVSTAT_END_TRUNCATE
} gsl_movstat_end_t;

/* accumulator struct
 * size          - return number of bytes needed for accumulator with maximum of n elements
 * init          - initialize accumulator state
 * insert        - insert a single sample into accumulator; if there are already n
 *                 samples in accumulator, oldest sample is overwritten
 * delete_oldest - delete oldest sample from accumulator
 * get           - return accumulated value
 */
typedef struct
{
  size_t (*size) (const size_t n);
  int (*init) (const size_t n, void * vstate);
  int (*insert) (const double x, void * vstate);
  int (*delete_oldest) (void * vstate);
  int (*get) (void * params, double * result, const void * vstate);
} gsl_movstat_accum;

typedef struct
{
  double (* function) (const size_t n, double x[], void * params);
  void * params;
} gsl_movstat_function;

#define GSL_MOVSTAT_FN_EVAL(F,n,x) (*((F)->function))((n),(x),(F)->params)

/* workspace for moving window statistics */

typedef struct
{
  size_t H;          /* number of previous samples in window */
  size_t J;          /* number of after samples in window */
  size_t K;          /* window size K = H + J + 1 */
  double *work;      /* workspace, size K */
  void *state;       /* state workspace for various accumulators */
  size_t state_size; /* bytes allocated for 'state' */
} gsl_movstat_workspace;

/* alloc.c */

gsl_movstat_workspace *gsl_movstat_alloc(const size_t K);
gsl_movstat_workspace *gsl_movstat_alloc2(const size_t H, const size_t J);
gsl_movstat_workspace *gsl_movstat_alloc_with_size(const size_t accum_state_size, const size_t H, const size_t J);
void gsl_movstat_free(gsl_movstat_workspace * w);

/* apply.c */
int gsl_movstat_apply_accum(const gsl_movstat_end_t endtype, const gsl_vector * x,
                            const gsl_movstat_accum * accum, void * accum_params,
                            gsl_vector * y, gsl_vector * z,
                            gsl_movstat_workspace * w);
int gsl_movstat_apply(const gsl_movstat_end_t endtype, const gsl_movstat_function * F,
                      const gsl_vector * x, gsl_vector * y, gsl_movstat_workspace * w);

/* fill.c */
size_t gsl_movstat_fill(const gsl_movstat_end_t endtype, const gsl_vector * x, const size_t idx,
                        const size_t H, const size_t J, double * window);

int gsl_movstat_mean(const gsl_movstat_end_t endtype, const gsl_vector * x, gsl_vector * y, gsl_movstat_workspace * w);
int gsl_movstat_variance(const gsl_movstat_end_t endtype, const gsl_vector * x, gsl_vector * y, gsl_movstat_workspace * w);
int gsl_movstat_sd(const gsl_movstat_end_t endtype, const gsl_vector * x, gsl_vector * y, gsl_movstat_workspace * w);
int gsl_movstat_median(const gsl_movstat_end_t endtype, const gsl_vector * x, gsl_vector * y, gsl_movstat_workspace * w);
int gsl_movstat_min(const gsl_movstat_end_t endtype, const gsl_vector * x, gsl_vector * y, gsl_movstat_workspace * w);
int gsl_movstat_max(const gsl_movstat_end_t endtype, const gsl_vector * x, gsl_vector * y, gsl_movstat_workspace * w);
int gsl_movstat_minmax(const gsl_movstat_end_t endtype, const gsl_vector * x, gsl_vector * y_min, gsl_vector * y_max, gsl_movstat_workspace * w);
int gsl_movstat_mad0(const gsl_movstat_end_t endtype, const gsl_vector * x, gsl_vector * xmedian,
                     gsl_vector * xmad, gsl_movstat_workspace * w);
int gsl_movstat_mad(const gsl_movstat_end_t endtype, const gsl_vector * x, gsl_vector * xmedian,
                    gsl_vector * xmad, gsl_movstat_workspace * w);
int gsl_movstat_qqr(const gsl_movstat_end_t endtype, const gsl_vector * x, const double q,
                    gsl_vector * xqqr, gsl_movstat_workspace * w);
int gsl_movstat_Sn(const gsl_movstat_end_t endtype, const gsl_vector * x,
                   gsl_vector * xscale, gsl_movstat_workspace * w);
int gsl_movstat_Qn(const gsl_movstat_end_t endtype, const gsl_vector * x,
                   gsl_vector * xscale, gsl_movstat_workspace * w);
int gsl_movstat_sum(const gsl_movstat_end_t endtype, const gsl_vector * x, gsl_vector * y, gsl_movstat_workspace * w);

/* accumulator variables */

GSL_VAR const gsl_movstat_accum * gsl_movstat_accum_mad;
GSL_VAR const gsl_movstat_accum * gsl_movstat_accum_max;
GSL_VAR const gsl_movstat_accum * gsl_movstat_accum_mean;
GSL_VAR const gsl_movstat_accum * gsl_movstat_accum_median;
GSL_VAR const gsl_movstat_accum * gsl_movstat_accum_min;
GSL_VAR const gsl_movstat_accum * gsl_movstat_accum_minmax;
GSL_VAR const gsl_movstat_accum * gsl_movstat_accum_sd;
GSL_VAR const gsl_movstat_accum * gsl_movstat_accum_Sn;
GSL_VAR const gsl_movstat_accum * gsl_movstat_accum_sum;
GSL_VAR const gsl_movstat_accum * gsl_movstat_accum_Qn;
GSL_VAR const gsl_movstat_accum * gsl_movstat_accum_qqr;
GSL_VAR const gsl_movstat_accum * gsl_movstat_accum_userfunc;
GSL_VAR const gsl_movstat_accum * gsl_movstat_accum_variance;

__END_DECLS

#endif /* __GSL_MOVSTAT_H__ */
