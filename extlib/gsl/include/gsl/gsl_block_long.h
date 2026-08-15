/* block/gsl_block_long.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Gerard Jungman, Brian Gough
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

#ifndef __GSL_BLOCK_LONG_H__
#define __GSL_BLOCK_LONG_H__

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
#include <gsl/gsl_errno.h>

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

struct gsl_block_long_struct
{
  size_t size;
  long *data;
};

typedef struct gsl_block_long_struct gsl_block_long;

GSL_FUN gsl_block_long *gsl_block_long_alloc (const size_t n);
GSL_FUN gsl_block_long *gsl_block_long_calloc (const size_t n);
GSL_FUN void gsl_block_long_free (gsl_block_long * b);

GSL_FUN int gsl_block_long_fread (FILE * stream, gsl_block_long * b);
GSL_FUN int gsl_block_long_fwrite (FILE * stream, const gsl_block_long * b);
GSL_FUN int gsl_block_long_fscanf (FILE * stream, gsl_block_long * b);
GSL_FUN int gsl_block_long_fprintf (FILE * stream, const gsl_block_long * b, const char *format);

GSL_FUN int gsl_block_long_raw_fread (FILE * stream, long * b, const size_t n, const size_t stride);
GSL_FUN int gsl_block_long_raw_fwrite (FILE * stream, const long * b, const size_t n, const size_t stride);
GSL_FUN int gsl_block_long_raw_fscanf (FILE * stream, long * b, const size_t n, const size_t stride);
GSL_FUN int gsl_block_long_raw_fprintf (FILE * stream, const long * b, const size_t n, const size_t stride, const char *format);

GSL_FUN size_t gsl_block_long_size (const gsl_block_long * b);
GSL_FUN long * gsl_block_long_data (const gsl_block_long * b);

__END_DECLS

#endif /* __GSL_BLOCK_LONG_H__ */
