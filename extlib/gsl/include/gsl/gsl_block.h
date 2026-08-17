#ifndef __GSL_BLOCK_H__
#define __GSL_BLOCK_H__

#if !defined( GSL_FUN )
#  if !defined( GSL_DLL )
#    define GSL_FUN extern
#  elif defined( BUILD_GSL_DLL )
#    define GSL_FUN extern __declspec(dllexport)
#  else
#    define GSL_FUN extern __declspec(dllimport)
#  endif
#endif

#include <gsl/gsl_block_complex_long_double.h>
#include <gsl/gsl_block_complex_double.h>
#include <gsl/gsl_block_complex_float.h>

#include <gsl/gsl_block_long_double.h>
#include <gsl/gsl_block_double.h>
#include <gsl/gsl_block_float.h>

#include <gsl/gsl_block_ulong.h>
#include <gsl/gsl_block_long.h>

#include <gsl/gsl_block_uint.h>
#include <gsl/gsl_block_int.h>

#include <gsl/gsl_block_ushort.h>
#include <gsl/gsl_block_short.h>

#include <gsl/gsl_block_uchar.h>
#include <gsl/gsl_block_char.h>

#endif /* __GSL_BLOCK_H__ */
