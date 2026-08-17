#ifndef __GSL_STATISTICS_H__
#define __GSL_STATISTICS_H__

#if !defined( GSL_FUN )
#  if !defined( GSL_DLL )
#    define GSL_FUN extern
#  elif defined( BUILD_GSL_DLL )
#    define GSL_FUN extern __declspec(dllexport)
#  else
#    define GSL_FUN extern __declspec(dllimport)
#  endif
#endif

#include <gsl/gsl_statistics_long_double.h>
#include <gsl/gsl_statistics_double.h>
#include <gsl/gsl_statistics_float.h>

#include <gsl/gsl_statistics_ulong.h>
#include <gsl/gsl_statistics_long.h>

#include <gsl/gsl_statistics_uint.h>
#include <gsl/gsl_statistics_int.h>

#include <gsl/gsl_statistics_ushort.h>
#include <gsl/gsl_statistics_short.h>

#include <gsl/gsl_statistics_uchar.h>
#include <gsl/gsl_statistics_char.h>

#endif /* __GSL_STATISTICS_H__ */
