/* gsl_multilarge.h
 * 
 * Copyright (C) 2015 Patrick Alken
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

#ifndef __GSL_MULTILARGE_H__
#define __GSL_MULTILARGE_H__

#if !defined( GSL_FUN )
#  if !defined( GSL_DLL )
#    define GSL_FUN extern
#  elif defined( BUILD_GSL_DLL )
#    define GSL_FUN extern __declspec(dllexport)
#  else
#    define GSL_FUN extern __declspec(dllimport)
#  endif
#endif

#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_types.h>

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

/* iteration solver type */
typedef struct
{
  const char *name;
  void * (*alloc) (const size_t p);
  int (*reset) (void *);
  int (*accumulate) (gsl_matrix * X, gsl_vector * y,
                     void *);
  int (*solve) (const double lambda, gsl_vector * c,
                double * rnorm, double * snorm, void *);
  int (*rcond) (double * rcond, void *);
  int (*lcurve) (gsl_vector * reg_param, gsl_vector * rho,
                 gsl_vector * eta, void *);
  void (*free) (void *);
} gsl_multilarge_linear_type;

typedef struct
{
  const gsl_multilarge_linear_type * type;
  void * state;
  size_t p;
} gsl_multilarge_linear_workspace;

/* available types */
GSL_VAR const gsl_multilarge_linear_type * gsl_multilarge_linear_normal;
GSL_VAR const gsl_multilarge_linear_type * gsl_multilarge_linear_tsqr;

/*
 * Prototypes
 */
GSL_FUN gsl_multilarge_linear_workspace *
gsl_multilarge_linear_alloc(const gsl_multilarge_linear_type * T,
                            const size_t p);

GSL_FUN void gsl_multilarge_linear_free(gsl_multilarge_linear_workspace * w);

GSL_FUN const char *gsl_multilarge_linear_name(const gsl_multilarge_linear_workspace * w);

GSL_FUN int gsl_multilarge_linear_reset(gsl_multilarge_linear_workspace * w);

GSL_FUN int gsl_multilarge_linear_accumulate(gsl_matrix * X,
                                     gsl_vector * y,
                                     gsl_multilarge_linear_workspace * w);

GSL_FUN int gsl_multilarge_linear_solve(const double lambda, gsl_vector * c,
                                double * rnorm, double * snorm,
                                gsl_multilarge_linear_workspace * w);

GSL_FUN int gsl_multilarge_linear_rcond(double *rcond, gsl_multilarge_linear_workspace * w);

GSL_FUN int gsl_multilarge_linear_lcurve(gsl_vector * reg_param, gsl_vector * rho,
                                 gsl_vector * eta,
                                 gsl_multilarge_linear_workspace * w);

GSL_FUN int gsl_multilarge_linear_wstdform1 (const gsl_vector * L,
                                     const gsl_matrix * X,
                                     const gsl_vector * w,
                                     const gsl_vector * y,
                                     gsl_matrix * Xs,
                                     gsl_vector * ys,
                                     gsl_multilarge_linear_workspace * work);

GSL_FUN int gsl_multilarge_linear_stdform1 (const gsl_vector * L,
                                    const gsl_matrix * X,
                                    const gsl_vector * y,
                                    gsl_matrix * Xs,
                                    gsl_vector * ys,
                                    gsl_multilarge_linear_workspace * work);

GSL_FUN int gsl_multilarge_linear_L_decomp (gsl_matrix * L, gsl_vector * tau);

GSL_FUN int gsl_multilarge_linear_wstdform2 (const gsl_matrix * LQR,
                                     const gsl_vector * Ltau,
                                     const gsl_matrix * X,
                                     const gsl_vector * w,
                                     const gsl_vector * y,
                                     gsl_matrix * Xs,
                                     gsl_vector * ys,
                                     gsl_multilarge_linear_workspace * work);

GSL_FUN int gsl_multilarge_linear_stdform2 (const gsl_matrix * LQR,
                                    const gsl_vector * Ltau,
                                    const gsl_matrix * X,
                                    const gsl_vector * y,
                                    gsl_matrix * Xs,
                                    gsl_vector * ys,
                                    gsl_multilarge_linear_workspace * work);

GSL_FUN int gsl_multilarge_linear_genform1 (const gsl_vector * L,
                                    const gsl_vector * cs,
                                    gsl_vector * c,
                                    gsl_multilarge_linear_workspace * work);

GSL_FUN int gsl_multilarge_linear_genform2 (const gsl_matrix * LQR,
                                    const gsl_vector * Ltau,
                                    const gsl_vector * cs,
                                    gsl_vector * c,
                                    gsl_multilarge_linear_workspace * work);

__END_DECLS

#endif /* __GSL_MULTILARGE_H__ */
