/* interpolation/gsl_interp2d.h
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

#ifndef __GSL_INTERP2D_H__
#define __GSL_INTERP2D_H__

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

typedef struct {
    const char* name;
    unsigned int min_size;
    void * (*alloc)(size_t xsize, size_t ysize);
    int    (*init)(void *, const double xa[], const double ya[], const double za[], size_t xsize, size_t ysize);
    int    (*eval)(const void *, const double xa[], const double ya[], const double za[], size_t xsize, size_t ysize, double x, double y, gsl_interp_accel*, gsl_interp_accel*, double* z);
    int    (*eval_deriv_x) (const void *, const double xa[], const double ya[], const double za[], size_t xsize, size_t ysize, double x, double y, gsl_interp_accel*, gsl_interp_accel*, double* z_p);
    int    (*eval_deriv_y) (const void *, const double xa[], const double ya[], const double za[], size_t xsize, size_t ysize, double x, double y, gsl_interp_accel*, gsl_interp_accel*, double* z_p);
    int    (*eval_deriv_xx) (const void *, const double xa[], const double ya[], const double za[], size_t xsize, size_t ysize, double x, double y, gsl_interp_accel*, gsl_interp_accel*, double* z_pp);
    int    (*eval_deriv_xy) (const void *, const double xa[], const double ya[], const double za[], size_t xsize, size_t ysize, double x, double y, gsl_interp_accel*, gsl_interp_accel*, double* z_pp);
    int    (*eval_deriv_yy) (const void *, const double xa[], const double ya[], const double za[], size_t xsize, size_t ysize, double x, double y, gsl_interp_accel*, gsl_interp_accel*, double* z_pp);
    void   (*free)(void *);
} gsl_interp2d_type;

typedef struct {
    const gsl_interp2d_type * type; /* interpolation type */
    double xmin;                    /* minimum value of x for which data have been provided */
    double xmax;                    /* maximum value of x for which data have been provided */
    double ymin;                    /* minimum value of y for which data have been provided */
    double ymax;                    /* maximum value of y for which data have been provided */
    size_t xsize;                   /* number of x values provided */
    size_t ysize;                   /* number of y values provided */
    void * state;                   /* internal state object specific to the interpolation type */
} gsl_interp2d;

/* available types */
GSL_VAR const gsl_interp2d_type * gsl_interp2d_bilinear;
GSL_VAR const gsl_interp2d_type * gsl_interp2d_bicubic;

GSL_FUN gsl_interp2d * gsl_interp2d_alloc(const gsl_interp2d_type * T, const size_t xsize,
                                  const size_t ysize);

GSL_FUN const char * gsl_interp2d_name(const gsl_interp2d * interp);
GSL_FUN size_t gsl_interp2d_min_size(const gsl_interp2d * interp);
GSL_FUN size_t gsl_interp2d_type_min_size(const gsl_interp2d_type * T);
GSL_FUN int gsl_interp2d_set(const gsl_interp2d * interp, double zarr[],
                     const size_t i, const size_t j, const double z);
GSL_FUN double gsl_interp2d_get(const gsl_interp2d * interp, const double zarr[],
                        const size_t i, const size_t j);
GSL_FUN size_t gsl_interp2d_idx(const gsl_interp2d * interp,
                        const size_t i, const size_t j);
GSL_FUN int gsl_interp2d_init(gsl_interp2d * interp, const double xa[], const double ya[],
                      const double za[], const size_t xsize, const size_t ysize);
GSL_FUN void gsl_interp2d_free(gsl_interp2d * interp);

GSL_FUN double gsl_interp2d_eval(const gsl_interp2d * interp, const double xarr[],
                         const double yarr[], const double zarr[], const double x,
                         const double y, gsl_interp_accel * xa, gsl_interp_accel * ya);

GSL_FUN double gsl_interp2d_eval_extrap(const gsl_interp2d * interp,
                                const double xarr[], const double yarr[],
                                const double zarr[], const double x,
                                const double y, gsl_interp_accel * xa,
                                gsl_interp_accel * ya);

GSL_FUN int gsl_interp2d_eval_e(const gsl_interp2d * interp, const double xarr[],
                        const double yarr[], const double zarr[],
                        const double x, const double y, gsl_interp_accel* xa,
                        gsl_interp_accel* ya, double * z);

GSL_FUN int gsl_interp2d_eval_e_extrap(const gsl_interp2d * interp,
                               const double xarr[],
                               const double yarr[],
                               const double zarr[],
                               const double x,
                               const double y,
                               gsl_interp_accel * xa,
                               gsl_interp_accel * ya,
                               double * z);

GSL_FUN double gsl_interp2d_eval_deriv_x(const gsl_interp2d * interp, const double xarr[],
                                 const double yarr[], const double zarr[],
                                 const double x, const double y, gsl_interp_accel * xa,
                                 gsl_interp_accel * ya);

GSL_FUN int gsl_interp2d_eval_deriv_x_e(const gsl_interp2d * interp, const double xarr[],
                                const double yarr[], const double zarr[],
                                const double x, const double y,
                                gsl_interp_accel * xa, gsl_interp_accel * ya, double * z);

GSL_FUN double gsl_interp2d_eval_deriv_y(const gsl_interp2d * interp, const double xarr[],
                                 const double yarr[], const double zarr[],
                                 const double x, const double y,
                                 gsl_interp_accel* xa, gsl_interp_accel* ya);

GSL_FUN int gsl_interp2d_eval_deriv_y_e(const gsl_interp2d * interp, const double xarr[],
                                const double yarr[], const double zarr[],
                                const double x, const double y,
                                gsl_interp_accel * xa, gsl_interp_accel * ya, double * z);

GSL_FUN double gsl_interp2d_eval_deriv_xx(const gsl_interp2d * interp, const double xarr[],
                                  const double yarr[], const double zarr[],
                                  const double x, const double y,
                                  gsl_interp_accel * xa, gsl_interp_accel * ya);

GSL_FUN int gsl_interp2d_eval_deriv_xx_e(const gsl_interp2d * interp, const double xarr[],
                                 const double yarr[], const double zarr[],
                                 const double x, const double y,
                                 gsl_interp_accel * xa, gsl_interp_accel * ya, double * z);

GSL_FUN double gsl_interp2d_eval_deriv_yy(const gsl_interp2d * interp, const double xarr[],
                                  const double yarr[], const double zarr[],
                                  const double x, const double y,
                                  gsl_interp_accel * xa, gsl_interp_accel * ya);

GSL_FUN int gsl_interp2d_eval_deriv_yy_e(const gsl_interp2d * interp, const double xarr[],
                                 const double yarr[], const double zarr[],
                                 const double x, const double y,
                                 gsl_interp_accel * xa, gsl_interp_accel * ya, double * z);

GSL_FUN double gsl_interp2d_eval_deriv_xy(const gsl_interp2d * interp, const double xarr[],
                                  const double yarr[], const double zarr[],
                                  const double x, const double y,
                                  gsl_interp_accel * xa, gsl_interp_accel * ya);

GSL_FUN int gsl_interp2d_eval_deriv_xy_e(const gsl_interp2d * interp, const double xarr[],
                                 const double yarr[], const double zarr[],
                                 const double x, const double y,
                                 gsl_interp_accel * xa, gsl_interp_accel * ya, double * z);


__END_DECLS

#endif /* __GSL_INTERP2D_H__ */
