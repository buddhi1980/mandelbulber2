/* ode-initval/odeiv2.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Gerard Jungman
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

/* Author:  G. Jungman */
/* Modified by Tuomo Keskitalo */

#ifndef __GSL_ODEIV2_H__
#define __GSL_ODEIV2_H__

#if !defined( GSL_FUN )
#  if !defined( GSL_DLL )
#    define GSL_FUN extern
#  elif defined( BUILD_GSL_DLL )
#    define GSL_FUN extern __declspec(dllexport)
#  else
#    define GSL_FUN extern __declspec(dllimport)
#  endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_types.h>

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS          /* empty */
# define __END_DECLS            /* empty */
#endif

__BEGIN_DECLS
/* Description of a system of ODEs.
 *
 * y' = f(t,y) = dydt(t, y)
 *
 * The system is specified by giving the right-hand-side
 * of the equation and possibly a jacobian function.
 *
 * Some methods require the jacobian function, which calculates
 * the matrix dfdy and the vector dfdt. The matrix dfdy conforms
 * to the GSL standard, being a continuous range of floating point
 * values, in row-order.
 *
 * As with GSL function objects, user-supplied parameter
 * data is also present. 
 */
  typedef struct
{
  int (*function) (double t, const double y[], double dydt[], void *params);
  int (*jacobian) (double t, const double y[], double *dfdy, double dfdt[],
                   void *params);
  size_t dimension;
  void *params;
}
gsl_odeiv2_system;

/* Function evaluation macros */

#define GSL_ODEIV_FN_EVAL(S,t,y,f)  (*((S)->function))(t,y,f,(S)->params)
#define GSL_ODEIV_JA_EVAL(S,t,y,dfdy,dfdt)  (*((S)->jacobian))(t,y,dfdy,dfdt,(S)->params)

/* Type definitions */

typedef struct gsl_odeiv2_step_struct gsl_odeiv2_step;
typedef struct gsl_odeiv2_control_struct gsl_odeiv2_control;
typedef struct gsl_odeiv2_evolve_struct gsl_odeiv2_evolve;
typedef struct gsl_odeiv2_driver_struct gsl_odeiv2_driver;

/* Stepper object
 *
 * Opaque object for stepping an ODE system from t to t+h.
 * In general the object has some state which facilitates
 * iterating the stepping operation.
 */

typedef struct
{
  const char *name;
  int can_use_dydt_in;
  int gives_exact_dydt_out;
  void *(*alloc) (size_t dim);
  int (*apply) (void *state, size_t dim, double t, double h, double y[],
                double yerr[], const double dydt_in[], double dydt_out[],
                const gsl_odeiv2_system * dydt);
  int (*set_driver) (void *state, const gsl_odeiv2_driver * d);
  int (*reset) (void *state, size_t dim);
  unsigned int (*order) (void *state);
  void (*free) (void *state);
}
gsl_odeiv2_step_type;

struct gsl_odeiv2_step_struct
{
  const gsl_odeiv2_step_type *type;
  size_t dimension;
  void *state;
};

/* Available stepper types */

GSL_VAR const gsl_odeiv2_step_type *gsl_odeiv2_step_rk2;
GSL_VAR const gsl_odeiv2_step_type *gsl_odeiv2_step_rk4;
GSL_VAR const gsl_odeiv2_step_type *gsl_odeiv2_step_rkf45;
GSL_VAR const gsl_odeiv2_step_type *gsl_odeiv2_step_rkck;
GSL_VAR const gsl_odeiv2_step_type *gsl_odeiv2_step_rk8pd;
GSL_VAR const gsl_odeiv2_step_type *gsl_odeiv2_step_rk2imp;
GSL_VAR const gsl_odeiv2_step_type *gsl_odeiv2_step_rk4imp;
GSL_VAR const gsl_odeiv2_step_type *gsl_odeiv2_step_bsimp;
GSL_VAR const gsl_odeiv2_step_type *gsl_odeiv2_step_rk1imp;
GSL_VAR const gsl_odeiv2_step_type *gsl_odeiv2_step_msadams;
GSL_VAR const gsl_odeiv2_step_type *gsl_odeiv2_step_msbdf;

/* Stepper object methods */

GSL_FUN gsl_odeiv2_step *gsl_odeiv2_step_alloc (const gsl_odeiv2_step_type * T,
                                        size_t dim);
GSL_FUN int gsl_odeiv2_step_reset (gsl_odeiv2_step * s);
GSL_FUN void gsl_odeiv2_step_free (gsl_odeiv2_step * s);
GSL_FUN const char *gsl_odeiv2_step_name (const gsl_odeiv2_step * s);
GSL_FUN unsigned int gsl_odeiv2_step_order (const gsl_odeiv2_step * s);
GSL_FUN int gsl_odeiv2_step_apply (gsl_odeiv2_step * s, double t, double h,
                           double y[], double yerr[], const double dydt_in[],
                           double dydt_out[], const gsl_odeiv2_system * dydt);
GSL_FUN int gsl_odeiv2_step_set_driver (gsl_odeiv2_step * s,
                                const gsl_odeiv2_driver * d);

/* Step size control object. */

typedef struct
{
  const char *name;
  void *(*alloc) (void);
  int (*init) (void *state, double eps_abs, double eps_rel, double a_y,
               double a_dydt);
  int (*hadjust) (void *state, size_t dim, unsigned int ord, const double y[],
                  const double yerr[], const double yp[], double *h);
  int (*errlevel) (void *state, const double y, const double dydt,
                   const double h, const size_t ind, double *errlev);
  int (*set_driver) (void *state, const gsl_odeiv2_driver * d);
  void (*free) (void *state);
}
gsl_odeiv2_control_type;

struct gsl_odeiv2_control_struct
{
  const gsl_odeiv2_control_type *type;
  void *state;
};

/* Possible return values for an hadjust() evolution method */

#define GSL_ODEIV_HADJ_INC   1  /* step was increased */
#define GSL_ODEIV_HADJ_NIL   0  /* step unchanged     */
#define GSL_ODEIV_HADJ_DEC (-1) /* step decreased     */

/* General step size control methods.
 *
 * The hadjust() method controls the adjustment of
 * step size given the result of a step and the error.
 * Valid hadjust() methods must return one of the codes below.
 * errlevel function calculates the desired error level D0.
 *
 * The general data can be used by specializations
 * to store state and control their heuristics.
 */

GSL_FUN gsl_odeiv2_control *gsl_odeiv2_control_alloc (const gsl_odeiv2_control_type *
                                              T);
GSL_FUN int gsl_odeiv2_control_init (gsl_odeiv2_control * c, double eps_abs,
                             double eps_rel, double a_y, double a_dydt);
GSL_FUN void gsl_odeiv2_control_free (gsl_odeiv2_control * c);
GSL_FUN int gsl_odeiv2_control_hadjust (gsl_odeiv2_control * c, gsl_odeiv2_step * s,
                                const double y[], const double yerr[],
                                const double dydt[], double *h);
GSL_FUN const char *gsl_odeiv2_control_name (const gsl_odeiv2_control * c);
GSL_FUN int gsl_odeiv2_control_errlevel (gsl_odeiv2_control * c, const double y,
                                 const double dydt, const double h,
                                 const size_t ind, double *errlev);
GSL_FUN int gsl_odeiv2_control_set_driver (gsl_odeiv2_control * c,
                                   const gsl_odeiv2_driver * d);

/* Available control object constructors.
 *
 * The standard control object is a four parameter heuristic
 * defined as follows:
 *    D0 = eps_abs + eps_rel * (a_y |y| + a_dydt h |y'|)
 *    D1 = |yerr|
 *    q  = consistency order of method (q=4 for 4(5) embedded RK)
 *    S  = safety factor (0.9 say)
 *
 *                      /  (D0/D1)^(1/(q+1))  D0 >= D1
 *    h_NEW = S h_OLD * |
 *                      \  (D0/D1)^(1/q)      D0 < D1
 *
 * This encompasses all the standard error scaling methods.
 *
 * The y method is the standard method with a_y=1, a_dydt=0.
 * The yp method is the standard method with a_y=0, a_dydt=1.
 */

GSL_FUN gsl_odeiv2_control *gsl_odeiv2_control_standard_new (double eps_abs,
                                                     double eps_rel,
                                                     double a_y,
                                                     double a_dydt);
GSL_FUN gsl_odeiv2_control *gsl_odeiv2_control_y_new (double eps_abs, double eps_rel);
GSL_FUN gsl_odeiv2_control *gsl_odeiv2_control_yp_new (double eps_abs,
                                               double eps_rel);

/* This controller computes errors using different absolute errors for
 * each component
 *
 *    D0 = eps_abs * scale_abs[i] + eps_rel * (a_y |y| + a_dydt h |y'|)
 */

GSL_FUN gsl_odeiv2_control *gsl_odeiv2_control_scaled_new (double eps_abs,
                                                   double eps_rel, double a_y,
                                                   double a_dydt,
                                                   const double scale_abs[],
                                                   size_t dim);

/* Evolution object */

struct gsl_odeiv2_evolve_struct
{
  size_t dimension;
  double *y0;
  double *yerr;
  double *dydt_in;
  double *dydt_out;
  double last_step;
  unsigned long int count;
  unsigned long int failed_steps;
  const gsl_odeiv2_driver *driver;
};

/* Evolution object methods */

GSL_FUN gsl_odeiv2_evolve *gsl_odeiv2_evolve_alloc (size_t dim);
GSL_FUN int gsl_odeiv2_evolve_apply (gsl_odeiv2_evolve * e, gsl_odeiv2_control * con,
                             gsl_odeiv2_step * step,
                             const gsl_odeiv2_system * dydt, double *t,
                             double t1, double *h, double y[]);
GSL_FUN int gsl_odeiv2_evolve_apply_fixed_step (gsl_odeiv2_evolve * e,
                                        gsl_odeiv2_control * con,
                                        gsl_odeiv2_step * step,
                                        const gsl_odeiv2_system * dydt,
                                        double *t, const double h0,
                                        double y[]);
GSL_FUN int gsl_odeiv2_evolve_reset (gsl_odeiv2_evolve * e);
GSL_FUN void gsl_odeiv2_evolve_free (gsl_odeiv2_evolve * e);
GSL_FUN int gsl_odeiv2_evolve_set_driver (gsl_odeiv2_evolve * e,
                                  const gsl_odeiv2_driver * d);

/* Driver object
 *
 * This is a high level wrapper for step, control and
 * evolve objects. 
 */

struct gsl_odeiv2_driver_struct
{
  const gsl_odeiv2_system *sys; /* ODE system */
  gsl_odeiv2_step *s;           /* stepper object */
  gsl_odeiv2_control *c;        /* control object */
  gsl_odeiv2_evolve *e;         /* evolve object */
  double h;                     /* step size */
  double hmin;                  /* minimum step size allowed */
  double hmax;                  /* maximum step size allowed */
  unsigned long int n;          /* number of steps taken */
  unsigned long int nmax;       /* Maximum number of steps allowed */
};

/* Driver object methods */

GSL_FUN gsl_odeiv2_driver *gsl_odeiv2_driver_alloc_y_new (const gsl_odeiv2_system *
                                                  sys,
                                                  const gsl_odeiv2_step_type *
                                                  T, const double hstart,
                                                  const double epsabs,
                                                  const double epsrel);
GSL_FUN gsl_odeiv2_driver *gsl_odeiv2_driver_alloc_yp_new (const gsl_odeiv2_system *
                                                   sys,
                                                   const gsl_odeiv2_step_type
                                                   * T, const double hstart,
                                                   const double epsabs,
                                                   const double epsrel);
GSL_FUN gsl_odeiv2_driver *gsl_odeiv2_driver_alloc_scaled_new (const gsl_odeiv2_system
                                                       * sys,
                                                       const
                                                       gsl_odeiv2_step_type *
                                                       T, const double hstart,
                                                       const double epsabs,
                                                       const double epsrel,
                                                       const double a_y,
                                                       const double a_dydt,
                                                       const double
                                                       scale_abs[]);
GSL_FUN gsl_odeiv2_driver *gsl_odeiv2_driver_alloc_standard_new (const
                                                         gsl_odeiv2_system *
                                                         sys,
                                                         const
                                                         gsl_odeiv2_step_type
                                                         * T,
                                                         const double hstart,
                                                         const double epsabs,
                                                         const double epsrel,
                                                         const double a_y,
                                                         const double a_dydt);
GSL_FUN int gsl_odeiv2_driver_set_hmin (gsl_odeiv2_driver * d, const double hmin);
GSL_FUN int gsl_odeiv2_driver_set_hmax (gsl_odeiv2_driver * d, const double hmax);
GSL_FUN int gsl_odeiv2_driver_set_nmax (gsl_odeiv2_driver * d,
                                const unsigned long int nmax);
GSL_FUN int gsl_odeiv2_driver_apply (gsl_odeiv2_driver * d, double *t,
                             const double t1, double y[]);
GSL_FUN int gsl_odeiv2_driver_apply_fixed_step (gsl_odeiv2_driver * d, double *t,
                                        const double h,
                                        const unsigned long int n,
                                        double y[]);
GSL_FUN int gsl_odeiv2_driver_reset (gsl_odeiv2_driver * d);
GSL_FUN int gsl_odeiv2_driver_reset_hstart (gsl_odeiv2_driver * d, const double hstart);
GSL_FUN void gsl_odeiv2_driver_free (gsl_odeiv2_driver * state);

__END_DECLS
#endif /* __GSL_ODEIV2_H__ */
