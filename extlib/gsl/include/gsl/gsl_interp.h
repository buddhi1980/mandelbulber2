/* interpolation/gsl_interp.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2004 Gerard Jungman
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

/* Author:  G. Jungman
 */
#ifndef __GSL_INTERP_H__
#define __GSL_INTERP_H__

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
#include <gsl/gsl_inline.h>
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

/* evaluation accelerator */
typedef struct {
  size_t  cache;        /* cache of index   */
  size_t  miss_count;   /* keep statistics  */
  size_t  hit_count;
}
gsl_interp_accel;


/* interpolation object type */
typedef struct {
  const char * name;
  unsigned int min_size;
  void *  (*alloc) (size_t size);
  int     (*init)    (void *, const double xa[], const double ya[], size_t size);
  int     (*eval)    (const void *, const double xa[], const double ya[], size_t size, double x, gsl_interp_accel *, double * y);
  int     (*eval_deriv)  (const void *, const double xa[], const double ya[], size_t size, double x, gsl_interp_accel *, double * y_p);
  int     (*eval_deriv2) (const void *, const double xa[], const double ya[], size_t size, double x, gsl_interp_accel *, double * y_pp);
  int     (*eval_integ)  (const void *, const double xa[], const double ya[], size_t size, gsl_interp_accel *, double a, double b, double * result);
  void    (*free)         (void *);

} gsl_interp_type;


/* general interpolation object */
typedef struct {
  const gsl_interp_type * type;
  double  xmin;
  double  xmax;
  size_t  size;
  void * state;
} gsl_interp;


/* available types */
GSL_VAR const gsl_interp_type * gsl_interp_linear;
GSL_VAR const gsl_interp_type * gsl_interp_polynomial;
GSL_VAR const gsl_interp_type * gsl_interp_cspline;
GSL_VAR const gsl_interp_type * gsl_interp_cspline_periodic;
GSL_VAR const gsl_interp_type * gsl_interp_akima;
GSL_VAR const gsl_interp_type * gsl_interp_akima_periodic;
GSL_VAR const gsl_interp_type * gsl_interp_steffen;

GSL_FUN gsl_interp_accel *
gsl_interp_accel_alloc(void);

GSL_FUN int
gsl_interp_accel_reset (gsl_interp_accel * a);

GSL_FUN void
gsl_interp_accel_free(gsl_interp_accel * a);

GSL_FUN gsl_interp *
gsl_interp_alloc(const gsl_interp_type * T, size_t n);
     
GSL_FUN int
gsl_interp_init(gsl_interp * obj, const double xa[], const double ya[], size_t size);

GSL_FUN const char * gsl_interp_name(const gsl_interp * interp);
GSL_FUN unsigned int gsl_interp_min_size(const gsl_interp * interp);
GSL_FUN unsigned int gsl_interp_type_min_size(const gsl_interp_type * T);


GSL_FUN int
gsl_interp_eval_e(const gsl_interp * obj,
                  const double xa[], const double ya[], double x,
                  gsl_interp_accel * a, double * y);

GSL_FUN double
gsl_interp_eval(const gsl_interp * obj,
                const double xa[], const double ya[], double x,
                gsl_interp_accel * a);

GSL_FUN int
gsl_interp_eval_deriv_e(const gsl_interp * obj,
                        const double xa[], const double ya[], double x,
                        gsl_interp_accel * a,
                        double * d);

GSL_FUN double
gsl_interp_eval_deriv(const gsl_interp * obj,
                      const double xa[], const double ya[], double x,
                      gsl_interp_accel * a);

GSL_FUN int
gsl_interp_eval_deriv2_e(const gsl_interp * obj,
                         const double xa[], const double ya[], double x,
                         gsl_interp_accel * a,
                         double * d2);

GSL_FUN double
gsl_interp_eval_deriv2(const gsl_interp * obj,
                       const double xa[], const double ya[], double x,
                       gsl_interp_accel * a);

GSL_FUN int
gsl_interp_eval_integ_e(const gsl_interp * obj,
                        const double xa[], const double ya[],
                        double a, double b,
                        gsl_interp_accel * acc,
                        double * result);

GSL_FUN double
gsl_interp_eval_integ(const gsl_interp * obj,
                      const double xa[], const double ya[],
                      double a, double b,
                      gsl_interp_accel * acc);

GSL_FUN void
gsl_interp_free(gsl_interp * interp);

GSL_FUN INLINE_DECL size_t
gsl_interp_bsearch(const double x_array[], double x,
                   size_t index_lo, size_t index_hi);

#ifdef HAVE_INLINE

/* Perform a binary search of an array of values.
 * 
 * The parameters index_lo and index_hi provide an initial bracket,
 * and it is assumed that index_lo < index_hi. The resulting index
 * is guaranteed to be strictly less than index_hi and greater than
 * or equal to index_lo, so that the implicit bracket [index, index+1]
 * always corresponds to a region within the implicit value range of
 * the value array.
 *
 * Note that this means the relationship of 'x' to x_array[index]
 * and x_array[index+1] depends on the result region, i.e. the
 * behaviour at the boundaries may not correspond to what you
 * expect. We have the following complete specification of the
 * behaviour.
 * Suppose the input is x_array[] = { x0, x1, ..., xN }
 *    if ( x == x0 )           then  index == 0
 *    if ( x > x0 && x <= x1 ) then  index == 0, and sim. for other interior pts
 *    if ( x == xN )           then  index == N-1
 *    if ( x > xN )            then  index == N-1
 *    if ( x < x0 )            then  index == 0 
 */

INLINE_FUN size_t
gsl_interp_bsearch(const double x_array[], double x,
                   size_t index_lo, size_t index_hi)
{
  size_t ilo = index_lo;
  size_t ihi = index_hi;
  while(ihi > ilo + 1) {
    size_t i = (ihi + ilo)/2;
    if(x_array[i] > x)
      ihi = i;
    else
      ilo = i;
  }
  
  return ilo;
}
#endif

GSL_FUN INLINE_DECL size_t 
gsl_interp_accel_find(gsl_interp_accel * a, const double x_array[], size_t size, double x);

#ifdef HAVE_INLINE
INLINE_FUN size_t
gsl_interp_accel_find(gsl_interp_accel * a, const double xa[], size_t len, double x)
{
  size_t x_index = a->cache;
 
  if(x < xa[x_index]) {
    a->miss_count++;
    a->cache = gsl_interp_bsearch(xa, x, 0, x_index);
  }
  else if(x >= xa[x_index + 1]) {
    a->miss_count++;
    a->cache = gsl_interp_bsearch(xa, x, x_index, len-1);
  }
  else {
    a->hit_count++;
  }
  
  return a->cache;
}
#endif /* HAVE_INLINE */


__END_DECLS

#endif /* __GSL_INTERP_H__ */
