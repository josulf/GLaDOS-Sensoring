/**
 * @addtogroup	libMU_Stats
 * Library for simple statistical analysis
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef LIBMU_STATS_H_
#define LIBMU_STATS_H_

#include <stdint.h>
#include <libMU/timer.h>
#include <math.h>

/**
 * Statistical data collection structure
 */
typedef struct _libMU_Stats_uint32_t {
	uint64_t	sumsquare;		/* Sum of squared values */
	uint64_t	sum;			/* Sum of values */
	uint32_t	count;			/* Sample count */
	uint32_t	max;			/* Max value */
	uint32_t	min;			/* Min value */
} libMU_Stats_uint32_t;

/**
 * Initialize the statistical data collection variable
 * @param	var		Pointer to location of the statistical data collection variable
 */
static inline void	libMU_Stats_Initialize( volatile libMU_Stats_uint32_t* var )
{
	var->sumsquare = 0;
	var->sum = 0;
	var->count = 0;
	var->max = 0;
	var->min = UINT32_MAX;
}

/**
 * Add new sample to statistical data collection variable
 * @param	var		Pointer to location of the statistical data collection variable
 * @param	value	Sample value
 * @note
 * If the sample overflows the sum or sumsquare variables the sums and counts
 * are reset
 */
void	libMU_Stats_Update( libMU_Stats_uint32_t* var, uint32_t value );

/**
 * Get max value from statistical data collection variable
 * @param	var		Pointer to location of the statistical data collection variable
 * @return			Max value
 */
static inline uint32_t	libMU_Stats_GetMaxValue( volatile libMU_Stats_uint32_t* var )
{
	return var->max;
}

/**
 * Get min value from statistical data collection variable
 * @param	var		Pointer to location of the statistical data collection variable
 * @return			Min value
 */
static inline uint32_t	libMU_Stats_GetMinValue( volatile libMU_Stats_uint32_t* var )
{
	return var->min;
}

/**
 * Get mean value from statistical data collection variable
 * @param	var		Pointer to location of the statistical data collection variable
 * @return			Mean value
 */
static inline uint32_t	libMU_Stats_GetMeanValue( volatile libMU_Stats_uint32_t* var )
{
	uint64_t sum = var->sum; uint32_t count = var->count;
	return (uint32_t)( (sum + count/2) / count );
}

/**
 * Get variance value from statistical data collection variable
 * @param	var		Pointer to location of the statistical data collection variable
 * @return			Variance value
 */
static inline double	libMU_Stats_GetVariance( volatile libMU_Stats_uint32_t* var )
{
	uint64_t sum = var->sum; uint32_t count = var->count;
	uint64_t sumsq = var->sumsquare;
	if( (sumsq>>32) > 0 ) {
		double m = (double)sum / (double)count;
		double res = (double)sumsq / (double)count - m * m;
		return res;
	}else{
		double m = (sumsq * count - sum*sum);
		m/= (count*(uint64_t)count);
		return m;
	}
}

/**
 * Get standard deviation value from statistical data collection variable
 * @param	var		Pointer to location of the statistical data collection variable
 * @return			Standard deviation value
 */
static inline double	libMU_Stats_GetStdDev( volatile libMU_Stats_uint32_t* var )
{
	return sqrt( libMU_Stats_GetVariance(var) );
}

#endif /*LIBMU_STATS_H_*/
/**
 * @}
 */
