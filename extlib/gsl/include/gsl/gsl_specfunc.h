/* Author:  G. Jungman */

        
/* Convenience header */
#ifndef __GSL_SPECFUNC_H__
#define __GSL_SPECFUNC_H__

#if !defined( GSL_FUN )
#  if !defined( GSL_DLL )
#    define GSL_FUN extern
#  elif defined( BUILD_GSL_DLL )
#    define GSL_FUN extern __declspec(dllexport)
#  else
#    define GSL_FUN extern __declspec(dllimport)
#  endif
#endif

#include <gsl/gsl_sf.h>

#endif /* __GSL_SPECFUNC_H__ */
