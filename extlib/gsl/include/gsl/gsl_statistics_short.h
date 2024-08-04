/* statistics/gsl_statistics_short.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Jim Davies, Brian Gough
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

#ifndef __GSL_STATISTICS_SHORT_H__
#define __GSL_STATISTICS_SHORT_H__

#if !defined( GSL_FUN )
#  if !defined( GSL_DLL )
#    define GSL_FUN extern
#  elif defined( BUILD_GSL_DLL )
#    define GSL_FUN extern __declspec(dllexport)
#  else
#    define GSL_FUN extern __declspec(dllimport)
#  endif
#endif

#include <stddef.h>

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

GSL_FUN double gsl_stats_short_mean (const short data[], const size_t stride, const size_t n);
GSL_FUN double gsl_stats_short_variance (const short data[], const size_t stride, const size_t n);
GSL_FUN double gsl_stats_short_sd (const short data[], const size_t stride, const size_t n);
GSL_FUN double gsl_stats_short_variance_with_fixed_mean (const short data[], const size_t stride, const size_t n, const double mean);
GSL_FUN double gsl_stats_short_sd_with_fixed_mean (const short data[], const size_t stride, const size_t n, const double mean);
GSL_FUN double gsl_stats_short_tss (const short data[], const size_t stride, const size_t n);
GSL_FUN double gsl_stats_short_tss_m (const short data[], const size_t stride, const size_t n, const double mean);

GSL_FUN double gsl_stats_short_absdev (const short data[], const size_t stride, const size_t n);
GSL_FUN double gsl_stats_short_skew (const short data[], const size_t stride, const size_t n);
GSL_FUN double gsl_stats_short_kurtosis (const short data[], const size_t stride, const size_t n);
GSL_FUN double gsl_stats_short_lag1_autocorrelation (const short data[], const size_t stride, const size_t n);

GSL_FUN double gsl_stats_short_covariance (const short data1[], const size_t stride1,const short data2[], const size_t stride2, const size_t n);
GSL_FUN double gsl_stats_short_correlation (const short data1[], const size_t stride1,const short data2[], const size_t stride2, const size_t n);
GSL_FUN double gsl_stats_short_spearman (const short data1[], const size_t stride1, const short data2[], const size_t stride2, const size_t n, double work[]);

GSL_FUN double gsl_stats_short_variance_m (const short data[], const size_t stride, const size_t n, const double mean);
GSL_FUN double gsl_stats_short_sd_m (const short data[], const size_t stride, const size_t n, const double mean);
GSL_FUN double gsl_stats_short_absdev_m (const short data[], const size_t stride, const size_t n, const double mean);
GSL_FUN double gsl_stats_short_skew_m_sd (const short data[], const size_t stride, const size_t n, const double mean, const double sd);
GSL_FUN double gsl_stats_short_kurtosis_m_sd (const short data[], const size_t stride, const size_t n, const double mean, const double sd);
GSL_FUN double gsl_stats_short_lag1_autocorrelation_m (const short data[], const size_t stride, const size_t n, const double mean);

GSL_FUN double gsl_stats_short_covariance_m (const short data1[], const size_t stride1,const short data2[], const size_t stride2, const size_t n, const double mean1, const double mean2);


GSL_FUN double gsl_stats_short_pvariance (const short data1[], const size_t stride1, const size_t n1, const short data2[], const size_t stride2, const size_t n2);
GSL_FUN double gsl_stats_short_ttest (const short data1[], const size_t stride1, const size_t n1, const short data2[], const size_t stride2, const size_t n2);

GSL_FUN short gsl_stats_short_max (const short data[], const size_t stride, const size_t n);
GSL_FUN short gsl_stats_short_min (const short data[], const size_t stride, const size_t n);
GSL_FUN void gsl_stats_short_minmax (short * min, short * max, const short data[], const size_t stride, const size_t n);

GSL_FUN size_t gsl_stats_short_max_index (const short data[], const size_t stride, const size_t n);
GSL_FUN size_t gsl_stats_short_min_index (const short data[], const size_t stride, const size_t n);
GSL_FUN void gsl_stats_short_minmax_index (size_t * min_index, size_t * max_index, const short data[], const size_t stride, const size_t n);

GSL_FUN double gsl_stats_short_median_from_sorted_data (const short sorted_data[], const size_t stride, const size_t n) ;
GSL_FUN double gsl_stats_short_quantile_from_sorted_data (const short sorted_data[], const size_t stride, const size_t n, const double f) ;

__END_DECLS

#endif /* __GSL_STATISTICS_SHORT_H__ */
