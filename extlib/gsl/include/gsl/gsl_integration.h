/* integration/gsl_integration.h
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

#ifndef __GSL_INTEGRATION_H__
#define __GSL_INTEGRATION_H__

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
#include <gsl/gsl_math.h>

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

/* Workspace for adaptive integrators */

typedef struct
  {
    size_t limit;
    size_t size;
    size_t nrmax;
    size_t i;
    size_t maximum_level;
    double *alist;
    double *blist;
    double *rlist;
    double *elist;
    size_t *order;
    size_t *level;
  }
gsl_integration_workspace;

GSL_FUN gsl_integration_workspace *
  gsl_integration_workspace_alloc (const size_t n);

GSL_FUN void
  gsl_integration_workspace_free (gsl_integration_workspace * w);


/* Workspace for QAWS integrator */

typedef struct
{
  double alpha;
  double beta;
  int mu;
  int nu;
  double ri[25];
  double rj[25];
  double rg[25];
  double rh[25];
}
gsl_integration_qaws_table;

GSL_FUN gsl_integration_qaws_table * 
gsl_integration_qaws_table_alloc (double alpha, double beta, int mu, int nu);

GSL_FUN int
gsl_integration_qaws_table_set (gsl_integration_qaws_table * t,
                                double alpha, double beta, int mu, int nu);

GSL_FUN void
gsl_integration_qaws_table_free (gsl_integration_qaws_table * t);

/* Workspace for QAWO integrator */

enum gsl_integration_qawo_enum { GSL_INTEG_COSINE, GSL_INTEG_SINE };

typedef struct
{
  size_t n;
  double omega;
  double L;
  double par;
  enum gsl_integration_qawo_enum sine;
  double *chebmo;
}
gsl_integration_qawo_table;

GSL_FUN gsl_integration_qawo_table * 
gsl_integration_qawo_table_alloc (double omega, double L, 
                                  enum gsl_integration_qawo_enum sine,
                                  size_t n);

GSL_FUN int
gsl_integration_qawo_table_set (gsl_integration_qawo_table * t,
                                double omega, double L,
                                enum gsl_integration_qawo_enum sine);

GSL_FUN int
gsl_integration_qawo_table_set_length (gsl_integration_qawo_table * t,
                                       double L);

GSL_FUN void
gsl_integration_qawo_table_free (gsl_integration_qawo_table * t);


/* Definition of an integration rule */

typedef void gsl_integration_rule (const gsl_function * f,
                                   double a, double b,
                                   double *result, double *abserr,
                                   double *defabs, double *resabs);

GSL_FUN void gsl_integration_qk15 (const gsl_function * f, double a, double b,
                           double *result, double *abserr,
                           double *resabs, double *resasc);

GSL_FUN void gsl_integration_qk21 (const gsl_function * f, double a, double b,
                           double *result, double *abserr,
                           double *resabs, double *resasc);

GSL_FUN void gsl_integration_qk31 (const gsl_function * f, double a, double b,
                           double *result, double *abserr,
                           double *resabs, double *resasc);

GSL_FUN void gsl_integration_qk41 (const gsl_function * f, double a, double b,
                           double *result, double *abserr,
                           double *resabs, double *resasc);

GSL_FUN void gsl_integration_qk51 (const gsl_function * f, double a, double b,
                           double *result, double *abserr,
                           double *resabs, double *resasc);

GSL_FUN void gsl_integration_qk61 (const gsl_function * f, double a, double b,
                           double *result, double *abserr,
                           double *resabs, double *resasc);

GSL_FUN void gsl_integration_qcheb (gsl_function * f, double a, double b, 
                            double *cheb12, double *cheb24);

/* The low-level integration rules in QUADPACK are identified by small
   integers (1-6). We'll use symbolic constants to refer to them.  */

enum
  {
    GSL_INTEG_GAUSS15 = 1,      /* 15 point Gauss-Kronrod rule */
    GSL_INTEG_GAUSS21 = 2,      /* 21 point Gauss-Kronrod rule */
    GSL_INTEG_GAUSS31 = 3,      /* 31 point Gauss-Kronrod rule */
    GSL_INTEG_GAUSS41 = 4,      /* 41 point Gauss-Kronrod rule */
    GSL_INTEG_GAUSS51 = 5,      /* 51 point Gauss-Kronrod rule */
    GSL_INTEG_GAUSS61 = 6       /* 61 point Gauss-Kronrod rule */
  };

GSL_FUN void 
gsl_integration_qk (const int n, const double xgk[], 
                    const double wg[], const double wgk[],
                    double fv1[], double fv2[],
                    const gsl_function *f, double a, double b,
                    double * result, double * abserr, 
                    double * resabs, double * resasc);


GSL_FUN int gsl_integration_qng (const gsl_function * f,
                         double a, double b,
                         double epsabs, double epsrel,
                         double *result, double *abserr,
                         size_t * neval);

GSL_FUN int gsl_integration_qag (const gsl_function * f,
                         double a, double b,
                         double epsabs, double epsrel, size_t limit,
                         int key,
                         gsl_integration_workspace * workspace,
                         double *result, double *abserr);

GSL_FUN int gsl_integration_qagi (gsl_function * f,
                          double epsabs, double epsrel, size_t limit,
                          gsl_integration_workspace * workspace,
                          double *result, double *abserr);

GSL_FUN int gsl_integration_qagiu (gsl_function * f,
                           double a,
                           double epsabs, double epsrel, size_t limit,
                           gsl_integration_workspace * workspace,
                           double *result, double *abserr);

GSL_FUN int gsl_integration_qagil (gsl_function * f,
                           double b,
                           double epsabs, double epsrel, size_t limit,
                           gsl_integration_workspace * workspace,
                           double *result, double *abserr);


GSL_FUN int gsl_integration_qags (const gsl_function * f,
                          double a, double b,
                          double epsabs, double epsrel, size_t limit,
                          gsl_integration_workspace * workspace,
                          double *result, double *abserr);

GSL_FUN int gsl_integration_qagp (const gsl_function * f,
                          double *pts, size_t npts,
                          double epsabs, double epsrel, size_t limit,
                          gsl_integration_workspace * workspace,
                          double *result, double *abserr);

GSL_FUN int gsl_integration_qawc (gsl_function *f,
                          const double a, const double b, const double c,
                          const double epsabs, const double epsrel, const size_t limit,
                          gsl_integration_workspace * workspace,
                          double * result, double * abserr);

GSL_FUN int gsl_integration_qaws (gsl_function * f,
                          const double a, const double b,
                          gsl_integration_qaws_table * t,
                          const double epsabs, const double epsrel,
                          const size_t limit,
                          gsl_integration_workspace * workspace,
                          double *result, double *abserr);

GSL_FUN int gsl_integration_qawo (gsl_function * f,
                          const double a,
                          const double epsabs, const double epsrel,
                          const size_t limit,
                          gsl_integration_workspace * workspace,
                          gsl_integration_qawo_table * wf,
                          double *result, double *abserr);

GSL_FUN int gsl_integration_qawf (gsl_function * f,
                          const double a,
                          const double epsabs,
                          const size_t limit,
                          gsl_integration_workspace * workspace,
                          gsl_integration_workspace * cycle_workspace,
                          gsl_integration_qawo_table * wf,
                          double *result, double *abserr);

/* Workspace for fixed-order Gauss-Legendre integration */

typedef struct
  {
    size_t n;         /* number of points */
    double *x;        /* Gauss abscissae/points */
    double *w;        /* Gauss weights for each abscissae */
    int precomputed;  /* high precision abscissae/weights precomputed? */
  }
gsl_integration_glfixed_table;


GSL_FUN gsl_integration_glfixed_table *
  gsl_integration_glfixed_table_alloc (size_t n);

GSL_FUN void
  gsl_integration_glfixed_table_free (gsl_integration_glfixed_table * t);

/* Routine for fixed-order Gauss-Legendre integration */

GSL_FUN double
  gsl_integration_glfixed (const gsl_function *f,
                           double a,
                           double b,
                           const gsl_integration_glfixed_table * t);

/* Routine to retrieve the i-th Gauss-Legendre point and weight from t */

GSL_FUN int
  gsl_integration_glfixed_point (double a,
                                 double b,
                                 size_t i,
                                 double *xi,
                                 double *wi,
                                 const gsl_integration_glfixed_table * t);


/* Cquad integration - Pedro Gonnet */

/* Data of a single interval */
typedef struct
{
  double a, b;
  double c[64];
  double fx[33];
  double igral, err;
  int depth, rdepth, ndiv;
} gsl_integration_cquad_ival;


/* The workspace is just a collection of intervals */
typedef struct
{
  size_t size;
  gsl_integration_cquad_ival *ivals;
  size_t *heap;
} gsl_integration_cquad_workspace;

GSL_FUN gsl_integration_cquad_workspace *
gsl_integration_cquad_workspace_alloc (const size_t n);

GSL_FUN void
gsl_integration_cquad_workspace_free (gsl_integration_cquad_workspace * w);

GSL_FUN int
gsl_integration_cquad (const gsl_function * f, double a, double b,
		                   double epsabs, double epsrel,
		                   gsl_integration_cquad_workspace * ws,
		                   double *result, double *abserr, size_t * nevals);

/* IQPACK related structures and routines */

typedef struct
{
  double alpha;
  double beta;
  double a;
  double b;
  double zemu;
  double shft;
  double slp;
  double al;
  double be;
} gsl_integration_fixed_params;

typedef struct
{
  int (*check)(const size_t n, const gsl_integration_fixed_params * params);
  int (*init)(const size_t n, double * diag, double * subdiag, gsl_integration_fixed_params * params);
} gsl_integration_fixed_type;

typedef struct
{
  size_t n;        /* number of nodes/weights */
  double *weights; /* quadrature weights */
  double *x;       /* quadrature nodes */
  double *diag;    /* diagonal of Jacobi matrix */
  double *subdiag; /* subdiagonal of Jacobi matrix */
  const gsl_integration_fixed_type * type;
} gsl_integration_fixed_workspace;

/* IQPACK integral types */
GSL_VAR const gsl_integration_fixed_type * gsl_integration_fixed_legendre;
GSL_VAR const gsl_integration_fixed_type * gsl_integration_fixed_chebyshev;
GSL_VAR const gsl_integration_fixed_type * gsl_integration_fixed_gegenbauer;
GSL_VAR const gsl_integration_fixed_type * gsl_integration_fixed_jacobi;
GSL_VAR const gsl_integration_fixed_type * gsl_integration_fixed_laguerre;
GSL_VAR const gsl_integration_fixed_type * gsl_integration_fixed_hermite;
GSL_VAR const gsl_integration_fixed_type * gsl_integration_fixed_exponential;
GSL_VAR const gsl_integration_fixed_type * gsl_integration_fixed_rational;
GSL_VAR const gsl_integration_fixed_type * gsl_integration_fixed_chebyshev2;

GSL_FUN gsl_integration_fixed_workspace *
gsl_integration_fixed_alloc(const gsl_integration_fixed_type * type, const size_t n,
                            const double a, const double b, const double alpha, const double beta);

GSL_FUN void gsl_integration_fixed_free(gsl_integration_fixed_workspace * w);

GSL_FUN size_t gsl_integration_fixed_n(const gsl_integration_fixed_workspace * w);

GSL_FUN double *gsl_integration_fixed_nodes(const gsl_integration_fixed_workspace * w);

GSL_FUN double *gsl_integration_fixed_weights(const gsl_integration_fixed_workspace * w);

GSL_FUN int gsl_integration_fixed(const gsl_function * func, double * result,
                          const gsl_integration_fixed_workspace * w);

__END_DECLS

#endif /* __GSL_INTEGRATION_H__ */
