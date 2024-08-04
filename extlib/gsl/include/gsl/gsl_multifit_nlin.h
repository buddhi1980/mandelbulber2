/* multifit_nlin/gsl_multifit_nlin.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Brian Gough
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

#ifndef __GSL_MULTIFIT_NLIN_H__
#define __GSL_MULTIFIT_NLIN_H__

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

GSL_FUN int gsl_multifit_gradient (const gsl_matrix * J, const gsl_vector * f,
                           gsl_vector * g);

GSL_FUN int gsl_multifit_covar (const gsl_matrix * J, const double epsrel, gsl_matrix * covar);
GSL_FUN int gsl_multifit_covar_QRPT (gsl_matrix * r, gsl_permutation * perm,
                             const double epsrel, gsl_matrix * covar);


/* Definition of vector-valued functions with parameters based on gsl_vector */

struct gsl_multifit_function_struct
{
  int (* f) (const gsl_vector * x, void * params, gsl_vector * f);
  size_t n;   /* number of functions */
  size_t p;   /* number of independent variables */
  void * params;
};

typedef struct gsl_multifit_function_struct gsl_multifit_function ;

#define GSL_MULTIFIT_FN_EVAL(F,x,y) (*((F)->f))(x,(F)->params,(y))

typedef struct
  {
    const char *name;
    size_t size;
    int (*alloc) (void *state, size_t n, size_t p);
    int (*set) (void *state, gsl_multifit_function * function, gsl_vector * x, gsl_vector * f, gsl_vector * dx);
    int (*iterate) (void *state, gsl_multifit_function * function, gsl_vector * x, gsl_vector * f, gsl_vector * dx);
    void (*free) (void *state);
  }
gsl_multifit_fsolver_type;

typedef struct
  {
    const gsl_multifit_fsolver_type * type;
    gsl_multifit_function * function ;
    gsl_vector * x ;
    gsl_vector * f ;
    gsl_vector * dx ;
    void *state;
  }
gsl_multifit_fsolver;

GSL_FUN gsl_multifit_fsolver *
gsl_multifit_fsolver_alloc (const gsl_multifit_fsolver_type * T, 
                            size_t n, size_t p);

GSL_FUN void gsl_multifit_fsolver_free (gsl_multifit_fsolver * s);

GSL_FUN int gsl_multifit_fsolver_set (gsl_multifit_fsolver * s, 
                              gsl_multifit_function * f, 
                              const gsl_vector * x);

GSL_FUN int gsl_multifit_fsolver_iterate (gsl_multifit_fsolver * s);

GSL_FUN int gsl_multifit_fsolver_driver (gsl_multifit_fsolver * s,
                                 const size_t maxiter,
                                 const double epsabs, const double epsrel);

GSL_FUN const char * gsl_multifit_fsolver_name (const gsl_multifit_fsolver * s);
GSL_FUN gsl_vector * gsl_multifit_fsolver_position (const gsl_multifit_fsolver * s);

/* Definition of vector-valued functions and gradient with parameters
   based on gsl_vector */

struct gsl_multifit_function_fdf_struct
{
  int (* f) (const gsl_vector * x, void * params, gsl_vector * f);
  int (* df) (const gsl_vector * x, void * params, gsl_matrix * df);
  int (* fdf) (const gsl_vector * x, void * params, gsl_vector * f, gsl_matrix *df);
  size_t n;       /* number of functions */
  size_t p;       /* number of independent variables */
  void * params;  /* user parameters */
  size_t nevalf;  /* number of function evaluations */
  size_t nevaldf; /* number of Jacobian evaluations */
};

typedef struct gsl_multifit_function_fdf_struct gsl_multifit_function_fdf ;

typedef struct
  {
    const char *name;
    size_t size;
    int (*alloc) (void *state, size_t n, size_t p);
    int (*set) (void *state, const gsl_vector * wts,
                gsl_multifit_function_fdf * fdf, gsl_vector * x,
                gsl_vector * f, gsl_vector * dx);
    int (*iterate) (void *state, const gsl_vector * wts,
                    gsl_multifit_function_fdf * fdf, gsl_vector * x,
                    gsl_vector * f, gsl_vector * dx);
    int (*gradient) (void *state, gsl_vector * g);
    int (*jac) (void *state, gsl_matrix * J);
    void (*free) (void *state);
  }
gsl_multifit_fdfsolver_type;

typedef struct
  {
    const gsl_multifit_fdfsolver_type * type;
    gsl_multifit_function_fdf * fdf ;
    gsl_vector * x;        /* parameter values x */
    gsl_vector * f;        /* residual vector f(x) */
    gsl_vector * dx;       /* step dx */
    gsl_vector * g;        /* gradient J^T f */
    gsl_vector * sqrt_wts; /* sqrt(wts) */
    size_t niter;          /* number of iterations performed */
    void *state;
  }
gsl_multifit_fdfsolver;


GSL_FUN gsl_multifit_fdfsolver *
gsl_multifit_fdfsolver_alloc (const gsl_multifit_fdfsolver_type * T, 
                              size_t n, size_t p);

GSL_FUN int
gsl_multifit_fdfsolver_set (gsl_multifit_fdfsolver * s, 
                            gsl_multifit_function_fdf * fdf,
                            const gsl_vector * x);
GSL_FUN int gsl_multifit_fdfsolver_wset (gsl_multifit_fdfsolver * s, 
                                 gsl_multifit_function_fdf * f, 
                                 const gsl_vector * x,
                                 const gsl_vector * wts);

GSL_FUN int
gsl_multifit_fdfsolver_iterate (gsl_multifit_fdfsolver * s);

GSL_FUN int gsl_multifit_fdfsolver_driver (gsl_multifit_fdfsolver * s,
                                   const size_t maxiter,
                                   const double xtol,
                                   const double gtol,
                                   const double ftol,
                                   int *info);

GSL_FUN int gsl_multifit_fdfsolver_jac (gsl_multifit_fdfsolver * s,
                                gsl_matrix * J);

GSL_FUN void
gsl_multifit_fdfsolver_free (gsl_multifit_fdfsolver * s);

GSL_FUN const char * gsl_multifit_fdfsolver_name (const gsl_multifit_fdfsolver * s);
GSL_FUN gsl_vector * gsl_multifit_fdfsolver_position (const gsl_multifit_fdfsolver * s);
GSL_FUN gsl_vector * gsl_multifit_fdfsolver_residual (const gsl_multifit_fdfsolver * s);
GSL_FUN size_t gsl_multifit_fdfsolver_niter (const gsl_multifit_fdfsolver * s);
GSL_FUN int gsl_multifit_eval_wf(gsl_multifit_function_fdf *fdf,
                         const gsl_vector *x, const gsl_vector *wts,
                         gsl_vector *y);
GSL_FUN int gsl_multifit_eval_wdf(gsl_multifit_function_fdf *fdf,
                          const gsl_vector *x, const gsl_vector *wts,
                          gsl_matrix *dy);

GSL_FUN int gsl_multifit_fdfsolver_test (const gsl_multifit_fdfsolver * s,
                                 const double xtol,
                                 const double gtol,
                                 const double ftol, int *info);
GSL_FUN int gsl_multifit_test_delta (const gsl_vector * dx, const gsl_vector * x, 
                             double epsabs, double epsrel);

GSL_FUN int gsl_multifit_test_gradient (const gsl_vector * g, double epsabs);

GSL_FUN int gsl_multifit_fdfsolver_dif_df(const gsl_vector *x,
                                  const gsl_vector *wts,
                                  gsl_multifit_function_fdf *fdf,
                                  const gsl_vector *f, gsl_matrix *J);
GSL_FUN int gsl_multifit_fdfsolver_dif_fdf(const gsl_vector *x, gsl_multifit_function_fdf *fdf,
                                   gsl_vector *f, gsl_matrix *J);

typedef struct
{
  size_t n;                         /* number of (original) residuals */
  size_t p;                         /* number of model parameters */
  double lambda;                    /* damping parameter */
  const gsl_vector *L_diag;         /* diagonal damping matrix or NULL */
  const gsl_matrix *L;              /* general damping matrix or NULL */
  gsl_vector *f;                    /* function values for finite diff J */
  gsl_vector *wts;                  /* weight vector for augmented system */
  gsl_multifit_fdfsolver * s;
  gsl_multifit_function_fdf *fdf;   /* user defined fdf */
  gsl_multifit_function_fdf fdftik; /* Tikhonov modified fdf */
} gsl_multifit_fdfridge;

GSL_FUN gsl_multifit_fdfridge *
gsl_multifit_fdfridge_alloc (const gsl_multifit_fdfsolver_type * T,
                             const size_t n, const size_t p);
GSL_FUN void gsl_multifit_fdfridge_free(gsl_multifit_fdfridge *work);
GSL_FUN const char *gsl_multifit_fdfridge_name(const gsl_multifit_fdfridge * w);
GSL_FUN gsl_vector *gsl_multifit_fdfridge_position (const gsl_multifit_fdfridge * w);
GSL_FUN gsl_vector *gsl_multifit_fdfridge_residual (const gsl_multifit_fdfridge * w);
GSL_FUN size_t gsl_multifit_fdfridge_niter (const gsl_multifit_fdfridge * w);
GSL_FUN int gsl_multifit_fdfridge_set (gsl_multifit_fdfridge * w,
                               gsl_multifit_function_fdf * f,
                               const gsl_vector * x,
                               const double lambda);
GSL_FUN int gsl_multifit_fdfridge_wset (gsl_multifit_fdfridge * w,
                                gsl_multifit_function_fdf * f,
                                const gsl_vector * x,
                                const double lambda,
                                const gsl_vector * wts);
GSL_FUN int gsl_multifit_fdfridge_set2 (gsl_multifit_fdfridge * w,
                                gsl_multifit_function_fdf * f,
                                const gsl_vector * x,
                                const gsl_vector * lambda);
GSL_FUN int gsl_multifit_fdfridge_wset2 (gsl_multifit_fdfridge * w,
                                 gsl_multifit_function_fdf * f,
                                 const gsl_vector * x,
                                 const gsl_vector * lambda,
                                 const gsl_vector * wts);
GSL_FUN int gsl_multifit_fdfridge_set3 (gsl_multifit_fdfridge * w,
                                gsl_multifit_function_fdf * f,
                                const gsl_vector * x,
                                const gsl_matrix * L);
GSL_FUN int gsl_multifit_fdfridge_wset3 (gsl_multifit_fdfridge * w,
                                 gsl_multifit_function_fdf * f,
                                 const gsl_vector * x,
                                 const gsl_matrix * L,
                                 const gsl_vector * wts);
GSL_FUN int gsl_multifit_fdfridge_iterate (gsl_multifit_fdfridge * w);
GSL_FUN int gsl_multifit_fdfridge_driver (gsl_multifit_fdfridge * w,
                                  const size_t maxiter,
                                  const double xtol,
                                  const double gtol,
                                  const double ftol,
                                  int *info);

/* extern const gsl_multifit_fsolver_type * gsl_multifit_fsolver_gradient; */

GSL_VAR const gsl_multifit_fdfsolver_type * gsl_multifit_fdfsolver_lmsder;
GSL_VAR const gsl_multifit_fdfsolver_type * gsl_multifit_fdfsolver_lmder;
GSL_VAR const gsl_multifit_fdfsolver_type * gsl_multifit_fdfsolver_lmniel;

__END_DECLS

#endif /* __GSL_MULTIFIT_NLIN_H__ */
