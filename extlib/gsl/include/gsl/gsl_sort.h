#ifndef __GSL_SORT_H__
#define __GSL_SORT_H__

#if !defined( GSL_FUN )
#  if !defined( GSL_DLL )
#    define GSL_FUN extern
#  elif defined( BUILD_GSL_DLL )
#    define GSL_FUN extern __declspec(dllexport)
#  else
#    define GSL_FUN extern __declspec(dllimport)
#  endif
#endif

#include <gsl/gsl_sort_long_double.h>
#include <gsl/gsl_sort_double.h>
#include <gsl/gsl_sort_float.h>

#include <gsl/gsl_sort_ulong.h>
#include <gsl/gsl_sort_long.h>

#include <gsl/gsl_sort_uint.h>
#include <gsl/gsl_sort_int.h>

#include <gsl/gsl_sort_ushort.h>
#include <gsl/gsl_sort_short.h>

#include <gsl/gsl_sort_uchar.h>
#include <gsl/gsl_sort_char.h>

#endif /* __GSL_SORT_H__ */
