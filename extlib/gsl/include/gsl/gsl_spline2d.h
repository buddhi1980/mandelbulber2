/* interpolation/gsl_spline2d.h
 * 
 * Copyright 2012 David Zaslavsky
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

#ifndef __GSL_SPLINE2D_H__
#define __GSL_SPLINE2D_H__

#if !defined( GSL_FUN )
#  if !defined( GSL_DLL )
#    define GSL_FUN extern
#  elif defined( BUILD_GSL_DLL )
#    define GSL_FUN extern __declspec(dllexport)
#  else
#    define GSL_FUN extern __declspec(dllimport)
#  endif
#endif

#include <gsl/gsl_interp.h>
#include <gsl/gsl_interp2d.h>

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
 * A 2D interpolation object which stores the arrays defining the function.
 * In all other respects, this is just like a gsl_interp2d object.
 */
typedef struct
{
  gsl_interp2d interp_object; /* low-level interpolation object */
  double * xarr;              /* x data array */
  double * yarr;              /* y data array */
  double * zarr;              /* z data array */
} gsl_spline2d;

GSL_FUN gsl_spline2d * gsl_spline2d_alloc(const gsl_interp2d_type * T, size_t xsize, size_t ysize);

GSL_FUN int gsl_spline2d_init(gsl_spline2d * interp, const double xa[],
                      const double ya[], const double za[],
                      size_t xsize, size_t ysize);

GSL_FUN void gsl_spline2d_free(gsl_spline2d * interp);

GSL_FUN double gsl_spline2d_eval(const gsl_spline2d * interp, const double x,
                         const double y, gsl_interp_accel* xa, gsl_interp_accel* ya);

GSL_FUN int gsl_spline2d_eval_e(const gsl_spline2d * interp, const double x,
                        const double y, gsl_interp_accel* xa, gsl_interp_accel* ya,
                        double * z);

GSL_FUN double gsl_spline2d_eval_deriv_x(const gsl_spline2d * interp, const double x,
                                 const double y, gsl_interp_accel* xa, gsl_interp_accel* ya);

GSL_FUN int gsl_spline2d_eval_deriv_x_e(const gsl_spline2d * interp, const double x,
                                const double y, gsl_interp_accel* xa,
                                gsl_interp_accel* ya, double * z);

GSL_FUN double gsl_spline2d_eval_deriv_y(const gsl_spline2d * interp, const double x,
                                 const double y, gsl_interp_accel* xa,
                                 gsl_interp_accel* ya);

GSL_FUN int gsl_spline2d_eval_deriv_y_e(const gsl_spline2d * interp, const double x,
                                const double y, gsl_interp_accel* xa,
                                gsl_interp_accel* ya, double * z);

GSL_FUN double gsl_spline2d_eval_deriv_xx(const gsl_spline2d * interp, const double x,
                                  const double y, gsl_interp_accel* xa, gsl_interp_accel* ya);

GSL_FUN int gsl_spline2d_eval_deriv_xx_e(const gsl_spline2d * interp, const double x,
                                 const double y, gsl_interp_accel* xa,
                                 gsl_interp_accel* ya, double * z);

GSL_FUN double gsl_spline2d_eval_deriv_yy(const gsl_spline2d * interp, const double x,
                                  const double y, gsl_interp_accel* xa, gsl_interp_accel* ya);

GSL_FUN int gsl_spline2d_eval_deriv_yy_e(const gsl_spline2d * interp, const double x,
                                 const double y, gsl_interp_accel* xa,
                                 gsl_interp_accel* ya, double * z);

GSL_FUN double gsl_spline2d_eval_deriv_xy(const gsl_spline2d * interp, const double x,
                                  const double y, gsl_interp_accel* xa, gsl_interp_accel* ya);

GSL_FUN int gsl_spline2d_eval_deriv_xy_e(const gsl_spline2d * interp, const double x,
                                 const double y, gsl_interp_accel* xa,
                                 gsl_interp_accel* ya, double * z);

GSL_FUN size_t gsl_spline2d_min_size(const gsl_spline2d * interp);

GSL_FUN const char * gsl_spline2d_name(const gsl_spline2d * interp);

GSL_FUN int gsl_spline2d_set(const gsl_spline2d * interp, double zarr[],
                     const size_t i, const size_t j, const double z);
GSL_FUN double gsl_spline2d_get(const gsl_spline2d * interp, const double zarr[],
                        const size_t i, const size_t j);

__END_DECLS

#endif /* __GSL_SPLINE2D_H__ */
