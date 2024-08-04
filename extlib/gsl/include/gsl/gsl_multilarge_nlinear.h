/* multilarge_nlinear/gsl_multilarge_nlinear.h
 * 
 * Copyright (C) 2015, 2016 Patrick Alken
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

#ifndef __GSL_MULTILARGE_NLINEAR_H__
#define __GSL_MULTILARGE_NLINEAR_H__

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
#include <gsl/gsl_types.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_blas.h>

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
  GSL_MULTILARGE_NLINEAR_FWDIFF,
  GSL_MULTILARGE_NLINEAR_CTRDIFF
} gsl_multilarge_nlinear_fdtype;

/* Definition of vector-valued functions and gradient with parameters
   based on gsl_vector */

typedef struct
{
  int (* f) (const gsl_vector * x, void * params, gsl_vector * f);
  int (* df) (CBLAS_TRANSPOSE_t TransJ, const gsl_vector * x,
              const gsl_vector * u, void * params, gsl_vector * v,
              gsl_matrix * JTJ);
  int (* fvv) (const gsl_vector * x, const gsl_vector * v, void * params,
               gsl_vector * fvv);
  size_t n;        /* number of functions */
  size_t p;        /* number of independent variables */
  void * params;   /* user parameters */
  size_t nevalf;   /* number of function evaluations */
  size_t nevaldfu; /* number of Jacobian matrix-vector evaluations */
  size_t nevaldf2; /* number of Jacobian J^T J evaluations */
  size_t nevalfvv; /* number of fvv evaluations */
} gsl_multilarge_nlinear_fdf;

/* trust region subproblem method */
typedef struct
{
  const char *name;
  void * (*alloc) (const void * params, const size_t n, const size_t p);
  int (*init) (const void * vtrust_state, void * vstate);
  int (*preloop) (const void * vtrust_state, void * vstate);
  int (*step) (const void * vtrust_state, const double delta,
               gsl_vector * dx, void * vstate);
  int (*preduction) (const void * vtrust_state, const gsl_vector * dx,
                     double * pred, void * vstate);
  void (*free) (void * vstate);
} gsl_multilarge_nlinear_trs;

/* scaling matrix specification */
typedef struct
{
  const char *name;
  int (*init) (const gsl_matrix * JTJ, gsl_vector * diag);
  int (*update) (const gsl_matrix * JTJ, gsl_vector * diag);
} gsl_multilarge_nlinear_scale;

/*
 * linear least squares solvers - there are three steps to
 * solving a least squares problem using a direct method:
 *
 * 1. init: called once per iteration when a new Jacobian matrix
 *          is required; form normal equations matrix J^T J
 * 2. presolve: called each time a new LM parameter value mu is available;
 *              used for cholesky method in order to factor
 *              the (J^T J + mu D^T D) matrix
 * 3. solve: solve the least square system for a given rhs
 */
typedef struct
{
  const char *name;
  void * (*alloc) (const size_t n, const size_t p);
  int (*init) (const void * vtrust_state, void * vstate);
  int (*presolve) (const double mu, const void * vtrust_state, void * vstate);
  int (*solve) (const gsl_vector * g, gsl_vector * x,
                const void * vtrust_state, void * vstate);
  int (*rcond) (double * rcond, const gsl_matrix * JTJ, void * vstate);
  int (*covar) (const gsl_matrix * JTJ, gsl_matrix * covar, void * vstate);
  void (*free) (void * vstate);
} gsl_multilarge_nlinear_solver;

/* tunable parameters */
typedef struct
{
  const gsl_multilarge_nlinear_trs *trs;       /* trust region subproblem method */
  const gsl_multilarge_nlinear_scale *scale;   /* scaling method */
  const gsl_multilarge_nlinear_solver *solver; /* solver method */
  gsl_multilarge_nlinear_fdtype fdtype;        /* finite difference method */
  double factor_up;                            /* factor for increasing trust radius */
  double factor_down;                          /* factor for decreasing trust radius */
  double avmax;                                /* max allowed |a|/|v| */
  double h_df;                                 /* step size for finite difference Jacobian */
  double h_fvv;                                /* step size for finite difference fvv */
  size_t max_iter;                             /* maximum iterations for trs method */
  double tol;                                  /* tolerance for solving trs */
} gsl_multilarge_nlinear_parameters;

typedef struct
{
  const char *name;
  void * (*alloc) (const gsl_multilarge_nlinear_parameters * params,
                   const size_t n, const size_t p);
  int (*init) (void * state, const gsl_vector * wts,
               gsl_multilarge_nlinear_fdf * fdf, const gsl_vector * x,
               gsl_vector * f, gsl_vector * g, gsl_matrix * JTJ);
  int (*iterate) (void * state, const gsl_vector * wts,
                  gsl_multilarge_nlinear_fdf * fdf, gsl_vector * x,
                  gsl_vector * f, gsl_vector * g, gsl_matrix * JTJ,
                  gsl_vector * dx);
  int (*rcond) (double * rcond, const gsl_matrix * JTJ, void * state);
  int (*covar) (const gsl_matrix * JTJ, gsl_matrix * covar, void * state);
  double (*avratio) (void * state);
  void (*free) (void * state);
} gsl_multilarge_nlinear_type;

/* current state passed to low-level trust region algorithms */
typedef struct
{
  const gsl_vector * x;             /* parameter values x */
  const gsl_vector * f;             /* residual vector f(x) */
  const gsl_vector * g;             /* gradient J^T f */
  const gsl_matrix * JTJ;           /* matrix J^T J */
  const gsl_vector * diag;          /* scaling matrix D */
  const gsl_vector * sqrt_wts;      /* sqrt(diag(W)) or NULL for unweighted */
  const double *mu;                 /* LM parameter */
  const gsl_multilarge_nlinear_parameters * params;
  void *solver_state;               /* workspace for direct least squares solver */
  gsl_multilarge_nlinear_fdf * fdf;
  double *avratio;                  /* |a| / |v| */
} gsl_multilarge_nlinear_trust_state;

typedef struct
{
  const gsl_multilarge_nlinear_type * type;
  gsl_multilarge_nlinear_fdf * fdf ;
  gsl_vector * x;             /* parameter values x */
  gsl_vector * f;             /* residual vector f(x) */
  gsl_vector * dx;            /* step dx */
  gsl_vector * g;             /* gradient J^T f */
  gsl_matrix * JTJ;           /* matrix J^T J */
  gsl_vector * sqrt_wts_work; /* sqrt(W) */
  gsl_vector * sqrt_wts;      /* ptr to sqrt_wts_work, or NULL if not using weights */
  size_t n;                   /* number of residuals */
  size_t p;                   /* number of parameters */
  size_t niter;               /* number of iterations performed */
  gsl_multilarge_nlinear_parameters params;
  void *state;
} gsl_multilarge_nlinear_workspace;

GSL_FUN gsl_multilarge_nlinear_workspace *
gsl_multilarge_nlinear_alloc (const gsl_multilarge_nlinear_type * T,
                              const gsl_multilarge_nlinear_parameters * params,
                              size_t n, size_t p);

GSL_FUN void gsl_multilarge_nlinear_free (gsl_multilarge_nlinear_workspace * w);

GSL_FUN gsl_multilarge_nlinear_parameters gsl_multilarge_nlinear_default_parameters(void);

GSL_FUN int
gsl_multilarge_nlinear_init (const gsl_vector * x,
                             gsl_multilarge_nlinear_fdf * fdf,
                             gsl_multilarge_nlinear_workspace * w);

GSL_FUN int gsl_multilarge_nlinear_winit (const gsl_vector * x,
                                  const gsl_vector * wts,
                                  gsl_multilarge_nlinear_fdf * fdf,
                                  gsl_multilarge_nlinear_workspace * w);

GSL_FUN int
gsl_multilarge_nlinear_iterate (gsl_multilarge_nlinear_workspace * w);

GSL_FUN double
gsl_multilarge_nlinear_avratio (const gsl_multilarge_nlinear_workspace * w);

GSL_FUN int
gsl_multilarge_nlinear_rcond (double * rcond, const gsl_multilarge_nlinear_workspace * w);

GSL_FUN int
gsl_multilarge_nlinear_covar (gsl_matrix * covar, gsl_multilarge_nlinear_workspace * w);

GSL_FUN int
gsl_multilarge_nlinear_driver (const size_t maxiter,
                               const double xtol,
                               const double gtol,
                               const double ftol,
                               void (*callback)(const size_t iter, void *params,
                                                const gsl_multilarge_nlinear_workspace *w),
                               void *callback_params,
                               int *info,
                               gsl_multilarge_nlinear_workspace * w);

GSL_FUN const char *
gsl_multilarge_nlinear_name (const gsl_multilarge_nlinear_workspace * w);

GSL_FUN gsl_vector *
gsl_multilarge_nlinear_position (const gsl_multilarge_nlinear_workspace * w);

GSL_FUN gsl_vector *
gsl_multilarge_nlinear_residual (const gsl_multilarge_nlinear_workspace * w);

GSL_FUN gsl_vector *
gsl_multilarge_nlinear_step (const gsl_multilarge_nlinear_workspace * w);

GSL_FUN size_t
gsl_multilarge_nlinear_niter (const gsl_multilarge_nlinear_workspace * w);

GSL_FUN const char *
gsl_multilarge_nlinear_trs_name (const gsl_multilarge_nlinear_workspace * w);

GSL_FUN int gsl_multilarge_nlinear_eval_f(gsl_multilarge_nlinear_fdf *fdf,
                                  const gsl_vector *x,
                                  const gsl_vector *swts,
                                  gsl_vector *y);

GSL_FUN int
gsl_multilarge_nlinear_eval_df(const CBLAS_TRANSPOSE_t TransJ,
                               const gsl_vector *x,
                               const gsl_vector *f,
                               const gsl_vector *u,
                               const gsl_vector *swts,
                               const double h,
                               const gsl_multilarge_nlinear_fdtype fdtype,
                               gsl_multilarge_nlinear_fdf *fdf,
                               gsl_vector *v,
                               gsl_matrix *JTJ,
                               gsl_vector *work);

GSL_FUN int
gsl_multilarge_nlinear_eval_fvv(const double h,
                                const gsl_vector *x,
                                const gsl_vector *v,
                                const gsl_vector *f,
                                const gsl_vector *swts,
                                gsl_multilarge_nlinear_fdf *fdf,
                                gsl_vector *yvv,
                                gsl_vector *work);

/* convergence.c */
GSL_FUN int
gsl_multilarge_nlinear_test (const double xtol, const double gtol,
                             const double ftol, int *info,
                             const gsl_multilarge_nlinear_workspace * w);

/* fdjac.c */
GSL_FUN int
gsl_multilarge_nlinear_df(const double h, const gsl_multilarge_nlinear_fdtype fdtype,
                          const gsl_vector *x, const gsl_vector *wts,
                          gsl_multilarge_nlinear_fdf *fdf,
                          const gsl_vector *f, gsl_matrix *J, gsl_vector *work);

/* fdfvv.c */
GSL_FUN int
gsl_multilarge_nlinear_fdfvv(const double h, const gsl_vector *x, const gsl_vector *v,
                             const gsl_vector *f, const gsl_matrix *J,
                             const gsl_vector *swts, gsl_multilarge_nlinear_fdf *fdf,
                             gsl_vector *fvv, gsl_vector *work);

/* top-level algorithms */
GSL_VAR const gsl_multilarge_nlinear_type * gsl_multilarge_nlinear_trust;

/* trust region subproblem methods */
GSL_VAR const gsl_multilarge_nlinear_trs * gsl_multilarge_nlinear_trs_lm;
GSL_VAR const gsl_multilarge_nlinear_trs * gsl_multilarge_nlinear_trs_lmaccel;
GSL_VAR const gsl_multilarge_nlinear_trs * gsl_multilarge_nlinear_trs_dogleg;
GSL_VAR const gsl_multilarge_nlinear_trs * gsl_multilarge_nlinear_trs_ddogleg;
GSL_VAR const gsl_multilarge_nlinear_trs * gsl_multilarge_nlinear_trs_subspace2D;
GSL_VAR const gsl_multilarge_nlinear_trs * gsl_multilarge_nlinear_trs_cgst;

/* scaling matrix strategies */
GSL_VAR const gsl_multilarge_nlinear_scale * gsl_multilarge_nlinear_scale_levenberg;
GSL_VAR const gsl_multilarge_nlinear_scale * gsl_multilarge_nlinear_scale_marquardt;
GSL_VAR const gsl_multilarge_nlinear_scale * gsl_multilarge_nlinear_scale_more;

/* linear solvers */
GSL_VAR const gsl_multilarge_nlinear_solver * gsl_multilarge_nlinear_solver_cholesky;
GSL_VAR const gsl_multilarge_nlinear_solver * gsl_multilarge_nlinear_solver_none;

__END_DECLS

#endif /* __GSL_MULTILARGE_NLINEAR_H__ */
