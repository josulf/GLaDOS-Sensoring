/**
 * @addtogroup libMU_StdLib
 * Library for low footprint implementations of standard C functions
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2012/6/11
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef LIBMU_STDLIB_H_
#define LIBMU_STDLIB_H_
#include <stdarg.h>

/**
 * Define this constant if you need floating point support
 */
#define	LIBMU_SNPRINTF_USE_FLOAT

/**
 * Define this constant if you want to use the math library log10() function
 * otherwise it will use multiplications and divisions to approximate the
 * integer part of the logarithm (only if LIBMU_SNPRINTF_USE_FLOAT is defined)
 */
//#define LIBMU_SNPRINTF_USE_LOG

/**
 * Low footprint snprintf implementation with OR without floating point support
 * @param	buf		Pointer to destination character array
 * @param	size	Size of destination character array
 * @param	format	printf() format specification
 * @param	...		Variable length argument
 * @return			Number of characters written into buf
 * @note			Not all format specifications are supported,
 * 					only %i, %d, %u, %x, %s and their variants with width
 * 					and right alignment.
 * 					If the constant LIBMU_SNPRINTF_USE_FLOAT is defined
 * 					the float version is used (%f and %g supported)
 * 					The width specification is always enforced (NONSTANDARD)
 */
int	libMU_snprintf( char* buf, unsigned int size, const char* format, ... );

/**
 * Low footprint snprintf implementation with OR without floating point support
 * @param	buf		Pointer to destination character array
 * @param	size	Size of destination character array
 * @param	format	printf() format specification
 * @param	vls		Variable length argument variable
 * @return			Number of characters written into buf
 * @see				libMU_snprintf()
 */
int	libMU_vsnprintf( char* buf, unsigned int size, const char* format, va_list vls );

/**
 * convert string to int32_t
 * @param	buf		Pointer to character array to convert
 * @return			Converted number
 * @note			Skips all initial whitespace
 * 					Converts until a nondigit character is found
 * 					(other than the initial '-' sign)
 */
int32_t libMU_atoi( char* buf );

/**
 * convert string to uint32_t
 * @param	buf		Pointer to character array to convert
 * @return			Converted number
 * @note			Skips all initial whitespace
 * 					Converts until a nondigit character is found
 */
uint32_t libMU_atoul( char* buf );

/**
 * convert string to int64_t
 * @param	buf		Pointer to character array to convert
 * @return			Converted number
 * @note			Skips all initial whitespace
 * 					Converts until a nondigit character is found
 * 					(other than the initial '-' sign)
 */
int64_t libMU_atoll( char* buf );

/**
 * convert string to uint64_t
 * @param	buf		Pointer to character array to convert
 * @return			Converted number
 * @note			Skips all initial whitespace
 * 					Converts until a nondigit character is found
 */
uint64_t libMU_atoull( char* buf );

#endif /*LIBMU_STDLIB_H_*/
/**
 * @}
 */
