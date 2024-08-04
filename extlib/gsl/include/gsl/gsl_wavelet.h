/* wavelet/gsl_wavelet.h
 * 
 * Copyright (C) 2004 Ivo Alxneit
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

#ifndef __GSL_WAVELET_H__
#define __GSL_WAVELET_H__

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
#include <gsl/gsl_errno.h>

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

typedef enum {
  gsl_wavelet_forward = 1, gsl_wavelet_backward = -1
} 
gsl_wavelet_direction;

typedef struct
{
  const char *name;
  int (*init) (const double **h1, const double **g1,
               const double **h2, const double **g2, size_t * nc,
               size_t * offset, size_t member);
}
gsl_wavelet_type;

typedef struct
{
  const gsl_wavelet_type *type;
  const double *h1;
  const double *g1;
  const double *h2;
  const double *g2;
  size_t nc;
  size_t offset;
}
gsl_wavelet;

typedef struct
{
  double *scratch;
  size_t n;
}
gsl_wavelet_workspace;

GSL_VAR const gsl_wavelet_type *gsl_wavelet_daubechies;
GSL_VAR const gsl_wavelet_type *gsl_wavelet_daubechies_centered;
GSL_VAR const gsl_wavelet_type *gsl_wavelet_haar;
GSL_VAR const gsl_wavelet_type *gsl_wavelet_haar_centered;
GSL_VAR const gsl_wavelet_type *gsl_wavelet_bspline;
GSL_VAR const gsl_wavelet_type *gsl_wavelet_bspline_centered;

GSL_FUN gsl_wavelet *gsl_wavelet_alloc (const gsl_wavelet_type * T, size_t k);
GSL_FUN void gsl_wavelet_free (gsl_wavelet * w);
GSL_FUN const char *gsl_wavelet_name (const gsl_wavelet * w);

GSL_FUN gsl_wavelet_workspace *gsl_wavelet_workspace_alloc (size_t n);
GSL_FUN void gsl_wavelet_workspace_free (gsl_wavelet_workspace * work);

GSL_FUN int gsl_wavelet_transform (const gsl_wavelet * w, 
                           double *data, size_t stride, size_t n,
                           gsl_wavelet_direction dir, 
                           gsl_wavelet_workspace * work);

GSL_FUN int gsl_wavelet_transform_forward (const gsl_wavelet * w, 
                                   double *data, size_t stride, size_t n, 
                                   gsl_wavelet_workspace * work);

GSL_FUN int gsl_wavelet_transform_inverse (const gsl_wavelet * w, 
                                    double *data, size_t stride, size_t n, 
                                    gsl_wavelet_workspace * work);

__END_DECLS

#endif /* __GSL_WAVELET_H__ */
