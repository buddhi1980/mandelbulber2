#ifndef __GSL_MATRIX_H__
#define __GSL_MATRIX_H__

#if !defined( GSL_FUN )
#  if !defined( GSL_DLL )
#    define GSL_FUN extern
#  elif defined( BUILD_GSL_DLL )
#    define GSL_FUN extern __declspec(dllexport)
#  else
#    define GSL_FUN extern __declspec(dllimport)
#  endif
#endif

#include <gsl/gsl_matrix_complex_long_double.h>
#include <gsl/gsl_matrix_complex_double.h>
#include <gsl/gsl_matrix_complex_float.h>

#include <gsl/gsl_matrix_long_double.h>
#include <gsl/gsl_matrix_double.h>
#include <gsl/gsl_matrix_float.h>

#include <gsl/gsl_matrix_ulong.h>
#include <gsl/gsl_matrix_long.h>

#include <gsl/gsl_matrix_uint.h>
#include <gsl/gsl_matrix_int.h>

#include <gsl/gsl_matrix_ushort.h>
#include <gsl/gsl_matrix_short.h>

#include <gsl/gsl_matrix_uchar.h>
#include <gsl/gsl_matrix_char.h>


#endif /* __GSL_MATRIX_H__ */
