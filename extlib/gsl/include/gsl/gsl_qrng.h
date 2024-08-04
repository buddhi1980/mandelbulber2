/* Author: G. Jungman + modifications from O. Teytaud
 */
#ifndef __GSL_QRNG_H__
#define __GSL_QRNG_H__

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
#include <gsl/gsl_types.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_inline.h>

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


/* Once again, more inane C-style OOP... kill me now. */

/* Structure describing a type of generator.
 */
typedef struct
{
  const char * name;
  unsigned int max_dimension;
  size_t (*state_size) (unsigned int dimension);
  int (*init_state) (void * state, unsigned int dimension);
  int (*get) (void * state, unsigned int dimension, double x[]);
}
gsl_qrng_type;

/* Structure describing a generator instance of a
 * specified type, with generator-specific state info
 * and dimension-specific info.
 */
typedef struct
{
  const gsl_qrng_type * type;
  unsigned int dimension;
  size_t state_size;
  void * state;
}
gsl_qrng;


/* Supported generator types.
 */
GSL_VAR const gsl_qrng_type * gsl_qrng_niederreiter_2;
GSL_VAR const gsl_qrng_type * gsl_qrng_sobol;
GSL_VAR const gsl_qrng_type * gsl_qrng_halton;
GSL_VAR const gsl_qrng_type * gsl_qrng_reversehalton;


/* Allocate and initialize a generator
 * of the specified type, in the given
 * space dimension.
 */
GSL_FUN gsl_qrng * gsl_qrng_alloc (const gsl_qrng_type * T, unsigned int dimension);


/* Copy a generator. */
GSL_FUN int gsl_qrng_memcpy (gsl_qrng * dest, const gsl_qrng * src);


/* Clone a generator. */
GSL_FUN gsl_qrng * gsl_qrng_clone (const gsl_qrng * q);


/* Free a generator. */
GSL_FUN void gsl_qrng_free (gsl_qrng * q);


/* Intialize a generator. */
GSL_FUN void gsl_qrng_init (gsl_qrng * q);


/* Get the standardized name of the generator. */
GSL_FUN const char * gsl_qrng_name (const gsl_qrng * q);


/* ISN'T THIS CONFUSING FOR PEOPLE?
  WHAT IF SOMEBODY TRIES TO COPY WITH THIS ???
  */
GSL_FUN size_t gsl_qrng_size (const gsl_qrng * q);


GSL_FUN void * gsl_qrng_state (const gsl_qrng * q);


/* Retrieve next vector in sequence. */
GSL_FUN INLINE_DECL int gsl_qrng_get (const gsl_qrng * q, double x[]);

#ifdef HAVE_INLINE
INLINE_FUN int gsl_qrng_get (const gsl_qrng * q, double x[])
{
  return (q->type->get) (q->state, q->dimension, x);
}

#endif /* HAVE_INLINE */


__END_DECLS


#endif /* !__GSL_QRNG_H__ */
