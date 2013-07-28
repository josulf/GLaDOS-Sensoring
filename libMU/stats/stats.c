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
#include <stdint.h>
#include <math.h>
#include <libMU/stats.h>

/**
 * Add new sample to statistical data collection variable
 * @param	var		Pointer to location of the statistical data collection variable
 * @param	value	Sample value
 * @note
 * If the sample overflows the sum or sumsquare variables the sums and counts
 * are reset
 */
void	libMU_Stats_Update( libMU_Stats_uint32_t* var, uint32_t value )
{
	uint64_t tmp = value * (uint64_t)value;
	uint64_t tmp_sumsq = var->sumsquare + tmp;
	uint64_t tmp_sum   = var->sum + value;
	var->count++;
	if( tmp_sumsq < var->sumsquare || tmp_sum < var->sum || var->count == 0 ) {
		var->sumsquare = tmp;	/* Reset if overflow */
		var->sum       = value;
		var->count     = 1;
	}else{
		var->sumsquare = tmp_sumsq;
		var->sum       = tmp_sum;
	}
	if( value > var->max ) var->max = value;
	if( value < var->min ) var->min = value;
}

/**
 * @}
 */
