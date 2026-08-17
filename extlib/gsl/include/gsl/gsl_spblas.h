/* gsl_spblas.h
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

#ifndef __GSL_SPBLAS_H__
#define __GSL_SPBLAS_H__

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
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_spmatrix.h>
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

/*
 * Prototypes
 */

GSL_FUN int gsl_spblas_dgemv(const CBLAS_TRANSPOSE_t TransA, const double alpha,
                     const gsl_spmatrix *A, const gsl_vector *x,
                     const double beta, gsl_vector *y);
GSL_FUN int gsl_spblas_dgemm(const double alpha, const gsl_spmatrix *A,
                     const gsl_spmatrix *B, gsl_spmatrix *C);
GSL_FUN size_t gsl_spblas_scatter(const gsl_spmatrix *A, const size_t j,
                          const double alpha, size_t *w, double *x,
                          const size_t mark, gsl_spmatrix *C, size_t nz);

__END_DECLS

#endif /* __GSL_SPBLAS_H__ */
