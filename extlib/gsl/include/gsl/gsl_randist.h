/* randist/gsl_randist.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 James Theiler, Brian Gough
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

#ifndef __GSL_RANDIST_H__
#define __GSL_RANDIST_H__

#if !defined( GSL_FUN )
#  if !defined( GSL_DLL )
#    define GSL_FUN extern
#  elif defined( BUILD_GSL_DLL )
#    define GSL_FUN extern __declspec(dllexport)
#  else
#    define GSL_FUN extern __declspec(dllimport)
#  endif
#endif
#include <gsl/gsl_rng.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

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

GSL_FUN unsigned int gsl_ran_bernoulli (const gsl_rng * r, double p);
GSL_FUN double gsl_ran_bernoulli_pdf (const unsigned int k, double p);

GSL_FUN double gsl_ran_beta (const gsl_rng * r, const double a, const double b);
GSL_FUN double gsl_ran_beta_pdf (const double x, const double a, const double b);

GSL_FUN unsigned int gsl_ran_binomial (const gsl_rng * r, double p, unsigned int n);
GSL_FUN unsigned int gsl_ran_binomial_knuth (const gsl_rng * r, double p, unsigned int n);
GSL_FUN unsigned int gsl_ran_binomial_tpe (const gsl_rng * r, double p, unsigned int n);
GSL_FUN double gsl_ran_binomial_pdf (const unsigned int k, const double p, const unsigned int n);

GSL_FUN double gsl_ran_exponential (const gsl_rng * r, const double mu);
GSL_FUN double gsl_ran_exponential_pdf (const double x, const double mu);

GSL_FUN double gsl_ran_exppow (const gsl_rng * r, const double a, const double b);
GSL_FUN double gsl_ran_exppow_pdf (const double x, const double a, const double b);

GSL_FUN double gsl_ran_cauchy (const gsl_rng * r, const double a);
GSL_FUN double gsl_ran_cauchy_pdf (const double x, const double a);

GSL_FUN double gsl_ran_chisq (const gsl_rng * r, const double nu);
GSL_FUN double gsl_ran_chisq_pdf (const double x, const double nu);

GSL_FUN void gsl_ran_dirichlet (const gsl_rng * r, const size_t K, const double alpha[], double theta[]);
GSL_FUN double gsl_ran_dirichlet_pdf (const size_t K, const double alpha[], const double theta[]);
GSL_FUN double gsl_ran_dirichlet_lnpdf (const size_t K, const double alpha[], const double theta[]);

GSL_FUN double gsl_ran_erlang (const gsl_rng * r, const double a, const double n);
GSL_FUN double gsl_ran_erlang_pdf (const double x, const double a, const double n);

GSL_FUN double gsl_ran_fdist (const gsl_rng * r, const double nu1, const double nu2);
GSL_FUN double gsl_ran_fdist_pdf (const double x, const double nu1, const double nu2);

GSL_FUN double gsl_ran_flat (const gsl_rng * r, const double a, const double b);
GSL_FUN double gsl_ran_flat_pdf (double x, const double a, const double b);

GSL_FUN double gsl_ran_gamma (const gsl_rng * r, const double a, const double b);
GSL_FUN double gsl_ran_gamma_int (const gsl_rng * r, const unsigned int a);
GSL_FUN double gsl_ran_gamma_pdf (const double x, const double a, const double b);
GSL_FUN double gsl_ran_gamma_mt (const gsl_rng * r, const double a, const double b);
GSL_FUN double gsl_ran_gamma_knuth (const gsl_rng * r, const double a, const double b);

GSL_FUN double gsl_ran_gaussian (const gsl_rng * r, const double sigma);
GSL_FUN double gsl_ran_gaussian_ratio_method (const gsl_rng * r, const double sigma);
GSL_FUN double gsl_ran_gaussian_ziggurat (const gsl_rng * r, const double sigma);
GSL_FUN double gsl_ran_gaussian_pdf (const double x, const double sigma);

GSL_FUN double gsl_ran_ugaussian (const gsl_rng * r);
GSL_FUN double gsl_ran_ugaussian_ratio_method (const gsl_rng * r);
GSL_FUN double gsl_ran_ugaussian_pdf (const double x);

GSL_FUN double gsl_ran_gaussian_tail (const gsl_rng * r, const double a, const double sigma);
GSL_FUN double gsl_ran_gaussian_tail_pdf (const double x, const double a, const double sigma);

GSL_FUN double gsl_ran_ugaussian_tail (const gsl_rng * r, const double a);
GSL_FUN double gsl_ran_ugaussian_tail_pdf (const double x, const double a);

GSL_FUN void gsl_ran_bivariate_gaussian (const gsl_rng * r, double sigma_x, double sigma_y, double rho, double *x, double *y);
GSL_FUN double gsl_ran_bivariate_gaussian_pdf (const double x, const double y, const double sigma_x, const double sigma_y, const double rho);

GSL_FUN int gsl_ran_multivariate_gaussian (const gsl_rng * r, const gsl_vector * mu, const gsl_matrix * L, gsl_vector * result);
GSL_FUN int gsl_ran_multivariate_gaussian_log_pdf (const gsl_vector * x,
                                           const gsl_vector * mu,
                                           const gsl_matrix * L,
                                           double * result,
                                           gsl_vector * work);
GSL_FUN int gsl_ran_multivariate_gaussian_pdf (const gsl_vector * x,
                                       const gsl_vector * mu,
                                       const gsl_matrix * L,
                                       double * result,
                                       gsl_vector * work);
GSL_FUN int gsl_ran_multivariate_gaussian_mean (const gsl_matrix * X, gsl_vector * mu_hat);
GSL_FUN int gsl_ran_multivariate_gaussian_vcov (const gsl_matrix * X, gsl_matrix * sigma_hat);

GSL_FUN double gsl_ran_landau (const gsl_rng * r);
GSL_FUN double gsl_ran_landau_pdf (const double x);

GSL_FUN unsigned int gsl_ran_geometric (const gsl_rng * r, const double p);
GSL_FUN double gsl_ran_geometric_pdf (const unsigned int k, const double p);

GSL_FUN unsigned int gsl_ran_hypergeometric (const gsl_rng * r, unsigned int n1, unsigned int n2, unsigned int t);
GSL_FUN double gsl_ran_hypergeometric_pdf (const unsigned int k, const unsigned int n1, const unsigned int n2, unsigned int t);

GSL_FUN double gsl_ran_gumbel1 (const gsl_rng * r, const double a, const double b);
GSL_FUN double gsl_ran_gumbel1_pdf (const double x, const double a, const double b);

GSL_FUN double gsl_ran_gumbel2 (const gsl_rng * r, const double a, const double b);
GSL_FUN double gsl_ran_gumbel2_pdf (const double x, const double a, const double b);

GSL_FUN double gsl_ran_logistic (const gsl_rng * r, const double a);
GSL_FUN double gsl_ran_logistic_pdf (const double x, const double a);

GSL_FUN double gsl_ran_lognormal (const gsl_rng * r, const double zeta, const double sigma);
GSL_FUN double gsl_ran_lognormal_pdf (const double x, const double zeta, const double sigma);

GSL_FUN unsigned int gsl_ran_logarithmic (const gsl_rng * r, const double p);
GSL_FUN double gsl_ran_logarithmic_pdf (const unsigned int k, const double p);

GSL_FUN void gsl_ran_multinomial (const gsl_rng * r, const size_t K,
                          const unsigned int N, const double p[],
                          unsigned int n[] );
GSL_FUN double gsl_ran_multinomial_pdf (const size_t K,
                                const double p[], const unsigned int n[] );
GSL_FUN double gsl_ran_multinomial_lnpdf (const size_t K,
                           const double p[], const unsigned int n[] );


GSL_FUN unsigned int gsl_ran_negative_binomial (const gsl_rng * r, double p, double n);
GSL_FUN double gsl_ran_negative_binomial_pdf (const unsigned int k, const double p, double n);

GSL_FUN unsigned int gsl_ran_pascal (const gsl_rng * r, double p, unsigned int n);
GSL_FUN double gsl_ran_pascal_pdf (const unsigned int k, const double p, unsigned int n);

GSL_FUN double gsl_ran_pareto (const gsl_rng * r, double a, const double b);
GSL_FUN double gsl_ran_pareto_pdf (const double x, const double a, const double b);

GSL_FUN unsigned int gsl_ran_poisson (const gsl_rng * r, double mu);
GSL_FUN void gsl_ran_poisson_array (const gsl_rng * r, size_t n, unsigned int array[],
                            double mu);
GSL_FUN double gsl_ran_poisson_pdf (const unsigned int k, const double mu);

GSL_FUN double gsl_ran_rayleigh (const gsl_rng * r, const double sigma);
GSL_FUN double gsl_ran_rayleigh_pdf (const double x, const double sigma);

GSL_FUN double gsl_ran_rayleigh_tail (const gsl_rng * r, const double a, const double sigma);
GSL_FUN double gsl_ran_rayleigh_tail_pdf (const double x, const double a, const double sigma);

GSL_FUN double gsl_ran_tdist (const gsl_rng * r, const double nu);
GSL_FUN double gsl_ran_tdist_pdf (const double x, const double nu);

GSL_FUN double gsl_ran_laplace (const gsl_rng * r, const double a);
GSL_FUN double gsl_ran_laplace_pdf (const double x, const double a);

GSL_FUN double gsl_ran_levy (const gsl_rng * r, const double c, const double alpha);
GSL_FUN double gsl_ran_levy_skew (const gsl_rng * r, const double c, const double alpha, const double beta);

GSL_FUN double gsl_ran_weibull (const gsl_rng * r, const double a, const double b);
GSL_FUN double gsl_ran_weibull_pdf (const double x, const double a, const double b);

GSL_FUN void gsl_ran_dir_2d (const gsl_rng * r, double * x, double * y);
GSL_FUN void gsl_ran_dir_2d_trig_method (const gsl_rng * r, double * x, double * y);
GSL_FUN void gsl_ran_dir_3d (const gsl_rng * r, double * x, double * y, double * z);
GSL_FUN void gsl_ran_dir_nd (const gsl_rng * r, size_t n, double * x);

GSL_FUN void gsl_ran_shuffle (const gsl_rng * r, void * base, size_t nmembm, size_t size);
GSL_FUN int gsl_ran_choose (const gsl_rng * r, void * dest, size_t k, void * src, size_t n, size_t size) ;
GSL_FUN void gsl_ran_sample (const gsl_rng * r, void * dest, size_t k, void * src, size_t n, size_t size) ;


typedef struct {                /* struct for Walker algorithm */
    size_t K;
    size_t *A;
    double *F;
} gsl_ran_discrete_t;

GSL_FUN gsl_ran_discrete_t * gsl_ran_discrete_preproc (size_t K, const double *P);
GSL_FUN void gsl_ran_discrete_free(gsl_ran_discrete_t *g);
GSL_FUN size_t gsl_ran_discrete (const gsl_rng *r, const gsl_ran_discrete_t *g);
GSL_FUN double gsl_ran_discrete_pdf (size_t k, const gsl_ran_discrete_t *g);


__END_DECLS

#endif /* __GSL_RANDIST_H__ */
