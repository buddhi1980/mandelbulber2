/* linalg/gsl_linalg.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2006, 2007 Gerard Jungman, Brian Gough, Patrick Alken
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

#ifndef __GSL_LINALG_H__
#define __GSL_LINALG_H__

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
#include <gsl/gsl_mode.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_inline.h>
#include <gsl/gsl_blas.h>

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
#define __BEGIN_DECLS extern "C" {
#define __END_DECLS }
#else
#define __BEGIN_DECLS           /* empty */
#define __END_DECLS             /* empty */
#endif

__BEGIN_DECLS

typedef enum
  {
    GSL_LINALG_MOD_NONE = 0,
    GSL_LINALG_MOD_TRANSPOSE = 1,
    GSL_LINALG_MOD_CONJUGATE = 2
  }
gsl_linalg_matrix_mod_t;


/* Note: You can now use the gsl_blas_dgemm function instead of matmult */

/* Simple implementation of matrix multiply.
 * Calculates C = A.B
 *
 * exceptions: GSL_EBADLEN
 */
GSL_FUN int gsl_linalg_matmult (const gsl_matrix * A,
                        const gsl_matrix * B,
                        gsl_matrix * C);


/* Simple implementation of matrix multiply.
 * Allows transposition of either matrix, so it
 * can compute A.B or Trans(A).B or A.Trans(B) or Trans(A).Trans(B)
 *
 * exceptions: GSL_EBADLEN
 */
GSL_FUN int gsl_linalg_matmult_mod (const gsl_matrix * A,
                            gsl_linalg_matrix_mod_t modA,
                            const gsl_matrix * B,
                            gsl_linalg_matrix_mod_t modB,
                            gsl_matrix * C);

/* Calculate the matrix exponential by the scaling and
 * squaring method described in Moler + Van Loan,
 * SIAM Rev 20, 801 (1978). The mode argument allows
 * choosing an optimal strategy, from the table
 * given in the paper, for a given precision.
 *
 * exceptions: GSL_ENOTSQR, GSL_EBADLEN
 */
GSL_FUN int gsl_linalg_exponential_ss(
  const gsl_matrix * A,
  gsl_matrix * eA,
  gsl_mode_t mode
  );


/* Householder Transformations */

GSL_FUN double gsl_linalg_householder_transform (gsl_vector * v);
GSL_FUN gsl_complex gsl_linalg_complex_householder_transform (gsl_vector_complex * v);

GSL_FUN int gsl_linalg_householder_hm (double tau, 
                               const gsl_vector * v, 
                               gsl_matrix * A);

GSL_FUN int gsl_linalg_householder_mh (double tau, 
                               const gsl_vector * v, 
                               gsl_matrix * A);

GSL_FUN int gsl_linalg_householder_hv (double tau, 
                               const gsl_vector * v, 
                               gsl_vector * w);

GSL_FUN int gsl_linalg_householder_hm1 (double tau, 
                                gsl_matrix * A);

GSL_FUN int gsl_linalg_complex_householder_hm (gsl_complex tau, 
                                       const gsl_vector_complex * v, 
                                       gsl_matrix_complex * A);

GSL_FUN int gsl_linalg_complex_householder_mh (gsl_complex tau,
                                       const gsl_vector_complex * v,
                                       gsl_matrix_complex * A);

GSL_FUN int gsl_linalg_complex_householder_hv (gsl_complex tau, 
                                       const gsl_vector_complex * v, 
                                       gsl_vector_complex * w);

/* Hessenberg reduction */

GSL_FUN int gsl_linalg_hessenberg_decomp(gsl_matrix *A, gsl_vector *tau);
GSL_FUN int gsl_linalg_hessenberg_unpack(gsl_matrix * H, gsl_vector * tau,
                                 gsl_matrix * U);
GSL_FUN int gsl_linalg_hessenberg_unpack_accum(gsl_matrix * H, gsl_vector * tau,
                                       gsl_matrix * U);
GSL_FUN int gsl_linalg_hessenberg_set_zero(gsl_matrix * H);
GSL_FUN int gsl_linalg_hessenberg_submatrix(gsl_matrix *M, gsl_matrix *A,
                                    size_t top, gsl_vector *tau);

/* Hessenberg-Triangular reduction */

GSL_FUN int gsl_linalg_hesstri_decomp(gsl_matrix * A, gsl_matrix * B,
                              gsl_matrix * U, gsl_matrix * V,
                              gsl_vector * work);

/* Singular Value Decomposition

 * exceptions: 
 */

GSL_FUN int
gsl_linalg_SV_decomp (gsl_matrix * A,
                      gsl_matrix * V,
                      gsl_vector * S,
                      gsl_vector * work);

GSL_FUN int
gsl_linalg_SV_decomp_mod (gsl_matrix * A,
                          gsl_matrix * X,
                          gsl_matrix * V,
                          gsl_vector * S,
                          gsl_vector * work);

GSL_FUN int gsl_linalg_SV_decomp_jacobi (gsl_matrix * A,
                                 gsl_matrix * Q,
                                 gsl_vector * S);

GSL_FUN int
gsl_linalg_SV_solve (const gsl_matrix * U,
                     const gsl_matrix * Q,
                     const gsl_vector * S,
                     const gsl_vector * b,
                     gsl_vector * x);

GSL_FUN int gsl_linalg_SV_leverage(const gsl_matrix *U, gsl_vector *h);


/* LU Decomposition, Gaussian elimination with partial pivoting
 */

GSL_FUN int gsl_linalg_LU_decomp (gsl_matrix * A, gsl_permutation * p, int *signum);

GSL_FUN int gsl_linalg_LU_solve (const gsl_matrix * LU,
                         const gsl_permutation * p,
                         const gsl_vector * b,
                         gsl_vector * x);

GSL_FUN int gsl_linalg_LU_svx (const gsl_matrix * LU,
                       const gsl_permutation * p,
                       gsl_vector * x);

GSL_FUN int gsl_linalg_LU_refine (const gsl_matrix * A,
                          const gsl_matrix * LU,
                          const gsl_permutation * p,
                          const gsl_vector * b,
                          gsl_vector * x,
                          gsl_vector * work);

GSL_FUN int gsl_linalg_LU_invert (const gsl_matrix * LU,
                          const gsl_permutation * p,
                          gsl_matrix * inverse);

GSL_FUN double gsl_linalg_LU_det (gsl_matrix * LU, int signum);
GSL_FUN double gsl_linalg_LU_lndet (gsl_matrix * LU);
GSL_FUN int gsl_linalg_LU_sgndet (gsl_matrix * lu, int signum);

/* Complex LU Decomposition */

GSL_FUN int gsl_linalg_complex_LU_decomp (gsl_matrix_complex * A, 
                                  gsl_permutation * p, 
                                  int *signum);

GSL_FUN int gsl_linalg_complex_LU_solve (const gsl_matrix_complex * LU,
                                 const gsl_permutation * p,
                                 const gsl_vector_complex * b,
                                 gsl_vector_complex * x);

GSL_FUN int gsl_linalg_complex_LU_svx (const gsl_matrix_complex * LU,
                               const gsl_permutation * p,
                               gsl_vector_complex * x);

GSL_FUN int gsl_linalg_complex_LU_refine (const gsl_matrix_complex * A,
                                  const gsl_matrix_complex * LU,
                                  const gsl_permutation * p,
                                  const gsl_vector_complex * b,
                                  gsl_vector_complex * x,
                                  gsl_vector_complex * work);

GSL_FUN int gsl_linalg_complex_LU_invert (const gsl_matrix_complex * LU,
                                  const gsl_permutation * p,
                                  gsl_matrix_complex * inverse);

GSL_FUN gsl_complex gsl_linalg_complex_LU_det (gsl_matrix_complex * LU,
                                       int signum);

GSL_FUN double gsl_linalg_complex_LU_lndet (gsl_matrix_complex * LU);

GSL_FUN gsl_complex gsl_linalg_complex_LU_sgndet (gsl_matrix_complex * LU,
                                          int signum);

/* QR decomposition */

GSL_FUN int gsl_linalg_QR_decomp (gsl_matrix * A,
                          gsl_vector * tau);

GSL_FUN int gsl_linalg_QR_solve (const gsl_matrix * QR,
                         const gsl_vector * tau,
                         const gsl_vector * b,
                         gsl_vector * x);

GSL_FUN int gsl_linalg_QR_svx (const gsl_matrix * QR,
                       const gsl_vector * tau,
                       gsl_vector * x);

GSL_FUN int gsl_linalg_QR_lssolve (const gsl_matrix * QR, 
                           const gsl_vector * tau, 
                           const gsl_vector * b, 
                           gsl_vector * x, 
                           gsl_vector * residual);


GSL_FUN int gsl_linalg_QR_QRsolve (gsl_matrix * Q,
                           gsl_matrix * R,
                           const gsl_vector * b,
                           gsl_vector * x);

GSL_FUN int gsl_linalg_QR_Rsolve (const gsl_matrix * QR,
                          const gsl_vector * b,
                          gsl_vector * x);

GSL_FUN int gsl_linalg_QR_Rsvx (const gsl_matrix * QR,
                        gsl_vector * x);

GSL_FUN int gsl_linalg_QR_update (gsl_matrix * Q,
                          gsl_matrix * R,
                          gsl_vector * w,
                          const gsl_vector * v);

GSL_FUN int gsl_linalg_QR_QTvec (const gsl_matrix * QR,
                         const gsl_vector * tau,
                         gsl_vector * v);

GSL_FUN int gsl_linalg_QR_Qvec (const gsl_matrix * QR,
                        const gsl_vector * tau,
                        gsl_vector * v);

GSL_FUN int gsl_linalg_QR_QTmat (const gsl_matrix * QR,
                         const gsl_vector * tau,
                         gsl_matrix * A);

GSL_FUN int gsl_linalg_QR_matQ (const gsl_matrix * QR,
                        const gsl_vector * tau,
                        gsl_matrix * A);

GSL_FUN int gsl_linalg_QR_unpack (const gsl_matrix * QR,
                          const gsl_vector * tau,
                          gsl_matrix * Q,
                          gsl_matrix * R);

GSL_FUN int gsl_linalg_R_solve (const gsl_matrix * R,
                        const gsl_vector * b,
                        gsl_vector * x);

GSL_FUN int gsl_linalg_R_svx (const gsl_matrix * R,
                      gsl_vector * x);


/* Q R P^T decomposition */

GSL_FUN int gsl_linalg_QRPT_decomp (gsl_matrix * A,
                            gsl_vector * tau,
                            gsl_permutation * p,
                            int *signum,
                            gsl_vector * norm);

GSL_FUN int gsl_linalg_QRPT_decomp2 (const gsl_matrix * A, 
                             gsl_matrix * q, gsl_matrix * r, 
                             gsl_vector * tau, 
                             gsl_permutation * p, 
                             int *signum,
                             gsl_vector * norm);

GSL_FUN int gsl_linalg_QRPT_solve (const gsl_matrix * QR,
                           const gsl_vector * tau,
                           const gsl_permutation * p,
                           const gsl_vector * b,
                           gsl_vector * x);

GSL_FUN int gsl_linalg_QRPT_lssolve (const gsl_matrix * QR,
                             const gsl_vector * tau,
                             const gsl_permutation * p,
                             const gsl_vector * b,
                             gsl_vector * x,
                             gsl_vector * residual);

GSL_FUN int gsl_linalg_QRPT_lssolve2 (const gsl_matrix * QR,
                              const gsl_vector * tau,
                              const gsl_permutation * p,
                              const gsl_vector * b,
                              const size_t rank,
                              gsl_vector * x,
                              gsl_vector * residual);

GSL_FUN int gsl_linalg_QRPT_svx (const gsl_matrix * QR,
                         const gsl_vector * tau,
                         const gsl_permutation * p,
                         gsl_vector * x);

GSL_FUN int gsl_linalg_QRPT_QRsolve (const gsl_matrix * Q,
                             const gsl_matrix * R,
                             const gsl_permutation * p,
                             const gsl_vector * b,
                             gsl_vector * x);

GSL_FUN int gsl_linalg_QRPT_Rsolve (const gsl_matrix * QR,
                             const gsl_permutation * p,
                             const gsl_vector * b,
                             gsl_vector * x);

GSL_FUN int gsl_linalg_QRPT_Rsvx (const gsl_matrix * QR,
                           const gsl_permutation * p,
                           gsl_vector * x);

GSL_FUN int gsl_linalg_QRPT_update (gsl_matrix * Q,
                            gsl_matrix * R,
                            const gsl_permutation * p,
                            gsl_vector * u,
                            const gsl_vector * v);

GSL_FUN size_t gsl_linalg_QRPT_rank (const gsl_matrix * QR, const double tol);

GSL_FUN int gsl_linalg_QRPT_rcond(const gsl_matrix * QR, double * rcond, gsl_vector * work);

/* COD decomposition */

GSL_FUN int gsl_linalg_COD_decomp(gsl_matrix * A, gsl_vector * tau_Q, gsl_vector * tau_Z,
                          gsl_permutation * p, size_t * rank, gsl_vector * work);

GSL_FUN int gsl_linalg_COD_decomp_e(gsl_matrix * A, gsl_vector * tau_Q, gsl_vector * tau_Z,
                            gsl_permutation * p, double tol, size_t * rank, gsl_vector * work);

GSL_FUN int gsl_linalg_COD_lssolve (const gsl_matrix * QRZT, const gsl_vector * tau_Q, const gsl_vector * tau_Z,
                            const gsl_permutation * perm, const size_t rank, const gsl_vector * b,
                            gsl_vector * x, gsl_vector * residual);

GSL_FUN int
gsl_linalg_COD_lssolve2 (const double lambda, const gsl_matrix * QRZT, const gsl_vector * tau_Q, const gsl_vector * tau_Z,
                         const gsl_permutation * perm, const size_t rank, const gsl_vector * b,
                         gsl_vector * x, gsl_vector * residual, gsl_matrix * S, gsl_vector * work);

GSL_FUN int gsl_linalg_COD_unpack(const gsl_matrix * QRZT, const gsl_vector * tau_Q,
                          const gsl_vector * tau_Z, const size_t rank, gsl_matrix * Q,
                          gsl_matrix * R, gsl_matrix * Z);

GSL_FUN int gsl_linalg_COD_matZ(const gsl_matrix * QRZT, const gsl_vector * tau_Z, const size_t rank,
                        gsl_matrix * A, gsl_vector * work);

/* LQ decomposition */

GSL_FUN int gsl_linalg_LQ_decomp (gsl_matrix * A, gsl_vector * tau);

GSL_FUN int gsl_linalg_LQ_solve_T (const gsl_matrix * LQ, const gsl_vector * tau, 
			 const gsl_vector * b, gsl_vector * x);

GSL_FUN int gsl_linalg_LQ_svx_T (const gsl_matrix * LQ, const gsl_vector * tau, 
                         gsl_vector * x);

GSL_FUN int gsl_linalg_LQ_lssolve_T (const gsl_matrix * LQ, const gsl_vector * tau, 
			   const gsl_vector * b, gsl_vector * x, 
			   gsl_vector * residual);

GSL_FUN int gsl_linalg_LQ_Lsolve_T (const gsl_matrix * LQ, const gsl_vector * b, 
			  gsl_vector * x);

GSL_FUN int gsl_linalg_LQ_Lsvx_T (const gsl_matrix * LQ, gsl_vector * x);

GSL_FUN int gsl_linalg_L_solve_T (const gsl_matrix * L, const gsl_vector * b, 
			gsl_vector * x);

GSL_FUN int gsl_linalg_LQ_vecQ (const gsl_matrix * LQ, const gsl_vector * tau, 
			gsl_vector * v);

GSL_FUN int gsl_linalg_LQ_vecQT (const gsl_matrix * LQ, const gsl_vector * tau, 
			 gsl_vector * v);

GSL_FUN int gsl_linalg_LQ_unpack (const gsl_matrix * LQ, const gsl_vector * tau, 
			  gsl_matrix * Q, gsl_matrix * L);

GSL_FUN int gsl_linalg_LQ_update (gsl_matrix * Q, gsl_matrix * R,
			  const gsl_vector * v, gsl_vector * w);
GSL_FUN int gsl_linalg_LQ_LQsolve (gsl_matrix * Q, gsl_matrix * L, 
			   const gsl_vector * b, gsl_vector * x);

/* P^T L Q decomposition */

GSL_FUN int gsl_linalg_PTLQ_decomp (gsl_matrix * A, gsl_vector * tau, 
			    gsl_permutation * p, int *signum, 
			    gsl_vector * norm);

GSL_FUN int gsl_linalg_PTLQ_decomp2 (const gsl_matrix * A, gsl_matrix * q, 
			     gsl_matrix * r, gsl_vector * tau, 
			     gsl_permutation * p, int *signum, 
			     gsl_vector * norm);

GSL_FUN int gsl_linalg_PTLQ_solve_T (const gsl_matrix * QR,
			   const gsl_vector * tau,
			   const gsl_permutation * p,
			   const gsl_vector * b,
			   gsl_vector * x);

GSL_FUN int gsl_linalg_PTLQ_svx_T (const gsl_matrix * LQ,
                           const gsl_vector * tau,
                           const gsl_permutation * p,
                           gsl_vector * x);

GSL_FUN int gsl_linalg_PTLQ_LQsolve_T (const gsl_matrix * Q, const gsl_matrix * L,
			     const gsl_permutation * p,
			     const gsl_vector * b,
			     gsl_vector * x);

GSL_FUN int gsl_linalg_PTLQ_Lsolve_T (const gsl_matrix * LQ,
			    const gsl_permutation * p,
			    const gsl_vector * b,
			    gsl_vector * x);

GSL_FUN int gsl_linalg_PTLQ_Lsvx_T (const gsl_matrix * LQ,
			  const gsl_permutation * p,
			  gsl_vector * x);

GSL_FUN int gsl_linalg_PTLQ_update (gsl_matrix * Q, gsl_matrix * L,
			    const gsl_permutation * p,
			    const gsl_vector * v, gsl_vector * w);

/* Cholesky Decomposition */

GSL_FUN int gsl_linalg_cholesky_decomp (gsl_matrix * A);
GSL_FUN int gsl_linalg_cholesky_decomp1 (gsl_matrix * A);

GSL_FUN int gsl_linalg_cholesky_solve (const gsl_matrix * cholesky,
                               const gsl_vector * b,
                               gsl_vector * x);

GSL_FUN int gsl_linalg_cholesky_svx (const gsl_matrix * cholesky,
                             gsl_vector * x);

GSL_FUN int gsl_linalg_cholesky_invert(gsl_matrix * cholesky);

/* Cholesky decomposition with unit-diagonal triangular parts.
 *   A = L D L^T, where diag(L) = (1,1,...,1).
 *   Upon exit, A contains L and L^T as for Cholesky, and
 *   the diagonal of A is (1,1,...,1). The vector Dis set
 *   to the diagonal elements of the diagonal matrix D.
 */
GSL_FUN int gsl_linalg_cholesky_decomp_unit(gsl_matrix * A, gsl_vector * D);

GSL_FUN int gsl_linalg_cholesky_scale(const gsl_matrix * A, gsl_vector * S);

GSL_FUN int gsl_linalg_cholesky_scale_apply(gsl_matrix * A, const gsl_vector * S);

GSL_FUN int gsl_linalg_cholesky_decomp2(gsl_matrix * A, gsl_vector * S);

GSL_FUN int gsl_linalg_cholesky_svx2 (const gsl_matrix * LLT,
                              const gsl_vector * S,
                              gsl_vector * x);

GSL_FUN int gsl_linalg_cholesky_solve2 (const gsl_matrix * LLT,
                                const gsl_vector * S,
                                const gsl_vector * b,
                                gsl_vector * x);

GSL_FUN int gsl_linalg_cholesky_rcond (const gsl_matrix * LLT, double * rcond,
                               gsl_vector * work);

/* Complex Cholesky Decomposition */

GSL_FUN int gsl_linalg_complex_cholesky_decomp (gsl_matrix_complex * A);

GSL_FUN int gsl_linalg_complex_cholesky_solve (const gsl_matrix_complex * cholesky,
                                       const gsl_vector_complex * b,
                                       gsl_vector_complex * x);

GSL_FUN int gsl_linalg_complex_cholesky_svx (const gsl_matrix_complex * cholesky,
                                     gsl_vector_complex * x);

GSL_FUN int gsl_linalg_complex_cholesky_invert(gsl_matrix_complex * cholesky);

/* Pivoted Cholesky LDLT decomposition */

GSL_FUN int gsl_linalg_pcholesky_decomp (gsl_matrix * A, gsl_permutation * p);

GSL_FUN int gsl_linalg_pcholesky_solve(const gsl_matrix * LDLT,
                               const gsl_permutation * p,
                               const gsl_vector * b,
                               gsl_vector * x);

GSL_FUN int gsl_linalg_pcholesky_svx(const gsl_matrix * LDLT,
                             const gsl_permutation * p,
                             gsl_vector * x);

GSL_FUN int gsl_linalg_pcholesky_decomp2(gsl_matrix * A, gsl_permutation * p,
                                 gsl_vector * S);

GSL_FUN int gsl_linalg_pcholesky_solve2(const gsl_matrix * LDLT,
                                const gsl_permutation * p,
                                const gsl_vector * S,
                                const gsl_vector * b,
                                gsl_vector * x);

GSL_FUN int gsl_linalg_pcholesky_svx2(const gsl_matrix * LDLT,
                              const gsl_permutation * p,
                              const gsl_vector * S,
                              gsl_vector * x);

GSL_FUN int gsl_linalg_pcholesky_invert(const gsl_matrix * LDLT, const gsl_permutation * p,
                                gsl_matrix * Ainv);

GSL_FUN int gsl_linalg_pcholesky_rcond (const gsl_matrix * LDLT, const gsl_permutation * p,
                                double * rcond, gsl_vector * work);

/* Modified Cholesky decomposition */

GSL_FUN int gsl_linalg_mcholesky_decomp (gsl_matrix * A, gsl_permutation * p,
                                 gsl_vector * E);

GSL_FUN int gsl_linalg_mcholesky_solve(const gsl_matrix * LDLT,
                               const gsl_permutation * p,
                               const gsl_vector * b,
                               gsl_vector * x);

GSL_FUN int gsl_linalg_mcholesky_svx(const gsl_matrix * LDLT,
                             const gsl_permutation * p,
                             gsl_vector * x);

GSL_FUN int gsl_linalg_mcholesky_rcond (const gsl_matrix * LDLT, const gsl_permutation * p,
                                double * rcond, gsl_vector * work);

GSL_FUN int gsl_linalg_mcholesky_invert(const gsl_matrix * LDLT, const gsl_permutation * p,
                                gsl_matrix * Ainv);

/* Symmetric to symmetric tridiagonal decomposition */

GSL_FUN int gsl_linalg_symmtd_decomp (gsl_matrix * A, 
                              gsl_vector * tau);

GSL_FUN int gsl_linalg_symmtd_unpack (const gsl_matrix * A, 
                              const gsl_vector * tau,
                              gsl_matrix * Q, 
                              gsl_vector * diag, 
                              gsl_vector * subdiag);

GSL_FUN int gsl_linalg_symmtd_unpack_T (const gsl_matrix * A,
                                gsl_vector * diag, 
                                gsl_vector * subdiag);

/* Hermitian to symmetric tridiagonal decomposition */

GSL_FUN int gsl_linalg_hermtd_decomp (gsl_matrix_complex * A, 
                              gsl_vector_complex * tau);

GSL_FUN int gsl_linalg_hermtd_unpack (const gsl_matrix_complex * A, 
                              const gsl_vector_complex * tau,
                              gsl_matrix_complex * U, 
                              gsl_vector * diag, 
                              gsl_vector * sudiag);

GSL_FUN int gsl_linalg_hermtd_unpack_T (const gsl_matrix_complex * A, 
                                gsl_vector * diag, 
                                gsl_vector * subdiag);

/* Linear Solve Using Householder Transformations

 * exceptions: 
 */

GSL_FUN int gsl_linalg_HH_solve (gsl_matrix * A, const gsl_vector * b, gsl_vector * x);
GSL_FUN int gsl_linalg_HH_svx (gsl_matrix * A, gsl_vector * x);

/* Linear solve for a symmetric tridiagonal system.

 * The input vectors represent the NxN matrix as follows:
 *
 *     diag[0]  offdiag[0]             0    ...
 *  offdiag[0]     diag[1]    offdiag[1]    ...
 *           0  offdiag[1]       diag[2]    ...
 *           0           0    offdiag[2]    ...
 *         ...         ...           ...    ...
 */
GSL_FUN int gsl_linalg_solve_symm_tridiag (const gsl_vector * diag,
                                   const gsl_vector * offdiag,
                                   const gsl_vector * b,
                                   gsl_vector * x);

/* Linear solve for a nonsymmetric tridiagonal system.

 * The input vectors represent the NxN matrix as follows:
 *
 *       diag[0]  abovediag[0]              0    ...
 *  belowdiag[0]       diag[1]   abovediag[1]    ...
 *             0  belowdiag[1]        diag[2]    ...
 *             0             0   belowdiag[2]    ...
 *           ...           ...            ...    ...
 */
GSL_FUN int gsl_linalg_solve_tridiag (const gsl_vector * diag,
                                   const gsl_vector * abovediag,
                                   const gsl_vector * belowdiag,
                                   const gsl_vector * b,
                                   gsl_vector * x);


/* Linear solve for a symmetric cyclic tridiagonal system.

 * The input vectors represent the NxN matrix as follows:
 *
 *      diag[0]  offdiag[0]             0   .....  offdiag[N-1]
 *   offdiag[0]     diag[1]    offdiag[1]   .....
 *            0  offdiag[1]       diag[2]   .....
 *            0           0    offdiag[2]   .....
 *          ...         ...
 * offdiag[N-1]         ...
 */
GSL_FUN int gsl_linalg_solve_symm_cyc_tridiag (const gsl_vector * diag,
                                       const gsl_vector * offdiag,
                                       const gsl_vector * b,
                                       gsl_vector * x);

/* Linear solve for a nonsymmetric cyclic tridiagonal system.

 * The input vectors represent the NxN matrix as follows:
 *
 *        diag[0]  abovediag[0]             0   .....  belowdiag[N-1]
 *   belowdiag[0]       diag[1]  abovediag[1]   .....
 *              0  belowdiag[1]       diag[2]
 *              0             0  belowdiag[2]   .....
 *            ...           ...
 * abovediag[N-1]           ...
 */
GSL_FUN int gsl_linalg_solve_cyc_tridiag (const gsl_vector * diag,
                                  const gsl_vector * abovediag,
                                  const gsl_vector * belowdiag,
                                  const gsl_vector * b,
                                  gsl_vector * x);


/* Bidiagonal decomposition */

GSL_FUN int gsl_linalg_bidiag_decomp (gsl_matrix * A, 
                              gsl_vector * tau_U, 
                              gsl_vector * tau_V);

GSL_FUN int gsl_linalg_bidiag_unpack (const gsl_matrix * A, 
                              const gsl_vector * tau_U, 
                              gsl_matrix * U, 
                              const gsl_vector * tau_V,
                              gsl_matrix * V,
                              gsl_vector * diag, 
                              gsl_vector * superdiag);

GSL_FUN int gsl_linalg_bidiag_unpack2 (gsl_matrix * A, 
                               gsl_vector * tau_U, 
                               gsl_vector * tau_V,
                               gsl_matrix * V);

GSL_FUN int gsl_linalg_bidiag_unpack_B (const gsl_matrix * A, 
                                gsl_vector * diag, 
                                gsl_vector * superdiag);

/* Balancing */

GSL_FUN int gsl_linalg_balance_matrix (gsl_matrix * A, gsl_vector * D);
GSL_FUN int gsl_linalg_balance_accum (gsl_matrix * A, gsl_vector * D);
GSL_FUN int gsl_linalg_balance_columns (gsl_matrix * A, gsl_vector * D);

/* condition estimation */

GSL_FUN int gsl_linalg_tri_upper_rcond(const gsl_matrix * A, double * rcond, gsl_vector * work);
GSL_FUN int gsl_linalg_tri_lower_rcond(const gsl_matrix * A, double * rcond, gsl_vector * work);
GSL_FUN int gsl_linalg_invnorm1(const size_t N,
                        int (* Ainvx)(CBLAS_TRANSPOSE_t TransA, gsl_vector * x, void * params),
                        void * params, double * Ainvnorm, gsl_vector * work);

/* triangular matrices */

GSL_FUN int gsl_linalg_tri_upper_invert(gsl_matrix * T);
GSL_FUN int gsl_linalg_tri_lower_invert(gsl_matrix * T);
GSL_FUN int gsl_linalg_tri_upper_unit_invert(gsl_matrix * T);
GSL_FUN int gsl_linalg_tri_lower_unit_invert(gsl_matrix * T);

GSL_FUN INLINE_DECL void gsl_linalg_givens (const double a, const double b,
                                    double *c, double *s);
GSL_FUN INLINE_DECL void gsl_linalg_givens_gv (gsl_vector * v, const size_t i,
                                       const size_t j, const double c,
                                       const double s);

#ifdef HAVE_INLINE

/* Generate a Givens rotation (cos,sin) which takes v=(x,y) to (|v|,0) 
   From Golub and Van Loan, "Matrix Computations", Section 5.1.8 */
INLINE_FUN
void
gsl_linalg_givens (const double a, const double b, double *c, double *s)
{
  if (b == 0)
    {
      *c = 1;
      *s = 0;
    }
  else if (fabs (b) > fabs (a))
    {
      double t = -a / b;
      double s1 = 1.0 / sqrt (1 + t * t);
      *s = s1;
      *c = s1 * t;
    }
  else
    {
      double t = -b / a;
      double c1 = 1.0 / sqrt (1 + t * t);
      *c = c1;
      *s = c1 * t;
    }
} /* gsl_linalg_givens() */

INLINE_FUN
void
gsl_linalg_givens_gv (gsl_vector * v, const size_t i, const size_t j,
                      const double c, const double s)
{
  /* Apply rotation to vector v' = G^T v */

  double vi = gsl_vector_get (v, i);
  double vj = gsl_vector_get (v, j);
  gsl_vector_set (v, i, c * vi - s * vj);
  gsl_vector_set (v, j, s * vi + c * vj);
}

#endif /* HAVE_INLINE */

__END_DECLS

#endif /* __GSL_LINALG_H__ */
