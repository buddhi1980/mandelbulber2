/* gsl_math.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2004, 2007 Gerard Jungman, Brian Gough
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

#ifndef __GSL_MATH_H__
#define __GSL_MATH_H__

#if !defined( GSL_FUN )
#  if !defined( GSL_DLL )
#    define GSL_FUN extern
#  elif defined( BUILD_GSL_DLL )
#    define GSL_FUN extern __declspec(dllexport)
#  else
#    define GSL_FUN extern __declspec(dllimport)
#  endif
#endif
#include <math.h>
#include <gsl/gsl_sys.h>
#include <gsl/gsl_inline.h>
#include <gsl/gsl_machine.h>
#include <gsl/gsl_precision.h>
#include <gsl/gsl_nan.h>
#include <gsl/gsl_pow_int.h>
#include <gsl/gsl_minmax.h>

#ifndef M_E
#define M_E        2.71828182845904523536028747135      /* e */
#endif

#ifndef M_LOG2E
#define M_LOG2E    1.44269504088896340735992468100      /* log_2 (e) */
#endif

#ifndef M_LOG10E
#define M_LOG10E   0.43429448190325182765112891892      /* log_10 (e) */
#endif

#ifndef M_SQRT2
#define M_SQRT2    1.41421356237309504880168872421      /* sqrt(2) */
#endif

#ifndef M_SQRT1_2
#define M_SQRT1_2  0.70710678118654752440084436210      /* sqrt(1/2) */
#endif


#ifndef M_SQRT3
#define M_SQRT3    1.73205080756887729352744634151      /* sqrt(3) */
#endif

#ifndef M_PI
#define M_PI       3.14159265358979323846264338328      /* pi */
#endif

#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923132169164      /* pi/2 */
#endif

#ifndef M_PI_4
#define M_PI_4     0.78539816339744830961566084582     /* pi/4 */
#endif

#ifndef M_SQRTPI
#define M_SQRTPI   1.77245385090551602729816748334      /* sqrt(pi) */
#endif

#ifndef M_2_SQRTPI
#define M_2_SQRTPI 1.12837916709551257389615890312      /* 2/sqrt(pi) */
#endif

#ifndef M_1_PI
#define M_1_PI     0.31830988618379067153776752675      /* 1/pi */
#endif

#ifndef M_2_PI
#define M_2_PI     0.63661977236758134307553505349      /* 2/pi */
#endif

#ifndef M_LN10
#define M_LN10     2.30258509299404568401799145468      /* ln(10) */
#endif

#ifndef M_LN2
#define M_LN2      0.69314718055994530941723212146      /* ln(2) */
#endif

#ifndef M_LNPI
#define M_LNPI     1.14472988584940017414342735135      /* ln(pi) */
#endif

#ifndef M_EULER
#define M_EULER    0.57721566490153286060651209008      /* Euler constant */
#endif

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

/* other needlessly compulsive abstractions */

#define GSL_IS_ODD(n)  ((n) & 1)
#define GSL_IS_EVEN(n) (!(GSL_IS_ODD(n)))
#define GSL_SIGN(x)    ((x) >= 0.0 ? 1 : -1)

/* Return nonzero if x is a real number, i.e. non NaN or infinite. */
#define GSL_IS_REAL(x) (gsl_finite(x))

/* Definition of an arbitrary function with parameters */

struct gsl_function_struct 
{
  double (* function) (double x, void * params);
  void * params;
};

typedef struct gsl_function_struct gsl_function ;

#define GSL_FN_EVAL(F,x) (*((F)->function))(x,(F)->params)

/* Definition of an arbitrary function returning two values, r1, r2 */

struct gsl_function_fdf_struct 
{
  double (* f) (double x, void * params);
  double (* df) (double x, void * params);
  void (* fdf) (double x, void * params, double * f, double * df);
  void * params;
};

typedef struct gsl_function_fdf_struct gsl_function_fdf ;

#define GSL_FN_FDF_EVAL_F(FDF,x) (*((FDF)->f))(x,(FDF)->params)
#define GSL_FN_FDF_EVAL_DF(FDF,x) (*((FDF)->df))(x,(FDF)->params)
#define GSL_FN_FDF_EVAL_F_DF(FDF,x,y,dy) (*((FDF)->fdf))(x,(FDF)->params,(y),(dy))


/* Definition of an arbitrary vector-valued function with parameters */

struct gsl_function_vec_struct 
{
  int (* function) (double x, double y[], void * params);
  void * params;
};

typedef struct gsl_function_vec_struct gsl_function_vec ;

#define GSL_FN_VEC_EVAL(F,x,y) (*((F)->function))(x,y,(F)->params)

__END_DECLS

#endif /* __GSL_MATH_H__ */
