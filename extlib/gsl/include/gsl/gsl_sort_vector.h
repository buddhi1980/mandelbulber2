#ifndef __GSL_SORT_VECTOR_H__
#define __GSL_SORT_VECTOR_H__

#if !defined( GSL_FUN )
#  if !defined( GSL_DLL )
#    define GSL_FUN extern
#  elif defined( BUILD_GSL_DLL )
#    define GSL_FUN extern __declspec(dllexport)
#  else
#    define GSL_FUN extern __declspec(dllimport)
#  endif
#endif

#include <gsl/gsl_sort_vector_long_double.h>
#include <gsl/gsl_sort_vector_double.h>
#include <gsl/gsl_sort_vector_float.h>

#include <gsl/gsl_sort_vector_ulong.h>
#include <gsl/gsl_sort_vector_long.h>

#include <gsl/gsl_sort_vector_uint.h>
#include <gsl/gsl_sort_vector_int.h>

#include <gsl/gsl_sort_vector_ushort.h>
#include <gsl/gsl_sort_vector_short.h>

#include <gsl/gsl_sort_vector_uchar.h>
#include <gsl/gsl_sort_vector_char.h>

#endif /* __GSL_SORT_VECTOR_H__ */
