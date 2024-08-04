/* gsl_splinalg.h
 * 
 * Copyright (C) 2012-2014 Patrick Alken
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

#ifndef __GSL_SPLINALG_H__
#define __GSL_SPLINALG_H__

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
#include <gsl/gsl_spmatrix.h>
#include <gsl/gsl_linalg.h>
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
  void * (*alloc) (const size_t n, const size_t m);
  int (*iterate) (const gsl_spmatrix *A, const gsl_vector *b,
                  const double tol, gsl_vector *x, void *);
  double (*normr)(const void *);
  void (*free) (void *);
} gsl_splinalg_itersolve_type;

typedef struct
{
  const gsl_splinalg_itersolve_type * type;
  double normr; /* current residual norm || b - A x || */
  void * state;
} gsl_splinalg_itersolve;

/* available types */
GSL_VAR const gsl_splinalg_itersolve_type * gsl_splinalg_itersolve_gmres;

/*
 * Prototypes
 */
GSL_FUN gsl_splinalg_itersolve *
gsl_splinalg_itersolve_alloc(const gsl_splinalg_itersolve_type *T,
                             const size_t n, const size_t m);
GSL_FUN void gsl_splinalg_itersolve_free(gsl_splinalg_itersolve *w);
GSL_FUN const char *gsl_splinalg_itersolve_name(const gsl_splinalg_itersolve *w);
GSL_FUN int gsl_splinalg_itersolve_iterate(const gsl_spmatrix *A,
                                   const gsl_vector *b,
                                   const double tol, gsl_vector *x,
                                   gsl_splinalg_itersolve *w);
GSL_FUN double gsl_splinalg_itersolve_normr(const gsl_splinalg_itersolve *w);

__END_DECLS

#endif /* __GSL_SPLINALG_H__ */
