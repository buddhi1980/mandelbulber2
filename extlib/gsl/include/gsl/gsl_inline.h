/* gsl_inline.h
 * 
 * Copyright (C) 2008, 2009 Brian Gough
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

#ifndef __GSL_INLINE_H__
#define __GSL_INLINE_H__

#if !defined( GSL_FUN )
#  if !defined( GSL_DLL )
#    define GSL_FUN extern
#  elif defined( BUILD_GSL_DLL )
#    define GSL_FUN extern __declspec(dllexport)
#  else
#    define GSL_FUN extern __declspec(dllimport)
#  endif
#endif

/* In recent versiions of GCC, the inline keyword has two different
   forms: GNU and C99.

   In GNU mode we can use 'extern inline' to make inline functions
   work like macros.  The function is only inlined--it is never output
   as a definition in an object file.

   In the new C99 mode 'extern inline' has a different meaning--it
   causes the definition of the function to be output in each object
   file where it is used.  This will result in multiple-definition
   errors on linking.  The 'inline' keyword on its own (without
   extern) has the same behavior as the original GNU 'extern inline'.

   The C99 style is the default with -std=c99 in GCC 4.3.  

   This header file allows either form of inline to be used by
   redefining the macros INLINE_DECL and INLINE_FUN.  These are used
   in the public header files as

        INLINE_DECL double gsl_foo (double x);
	#ifdef HAVE_INLINE
	INLINE_FUN double gsl_foo (double x) { return x+1.0; } ;
        #endif
   
*/

#ifdef HAVE_INLINE
#  if defined(__GNUC_STDC_INLINE__) || defined(GSL_C99_INLINE) || defined(HAVE_C99_INLINE)
#    define INLINE_DECL inline  /* use C99 inline */
#    define INLINE_FUN inline
#  else
#    define INLINE_DECL         /* use GNU extern inline */
#    define INLINE_FUN extern inline
#  endif
#else
#  define INLINE_DECL /* */
#endif

/* Range checking conditions in headers do not require any run-time
   tests of the global variable gsl_check_range.  They are enabled or
   disabled in user code at compile time with GSL_RANGE_CHECK macro.
   See also build.h. */
#define GSL_RANGE_COND(x) (x)

#endif /* __GSL_INLINE_H__ */
