/**
 * @addtogroup libMU_StdLib
 * Library for low footprint implementations of standard C functions
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>
#include <string.h>
#include <libMU/stdlib.h>

/**
 * Constants for snprintf()/vsnprintf() functions
 */
typedef enum _libMU_Stdlib_Constants_t {
	LIBMU_SNPRINTF_ERROR_CHAR = '#'
} libMU_Stdlib_Constants_t;

/**
 * Variable list argument variable output value
 */
va_list libMU_vsnprintf_vls_out;

/**
 * static implementations
 */
static char*	libMU_uint_to_str( char* buf, int size, unsigned int num, int width, char justify_right );
static char*	libMU_int_to_str( char* buf, int size, int num, int width, char justify_right );
static char*	libMU_uint_to_hex( char* buf, int size, unsigned int num, int width, char fill_char );
static char*	libMU_double_to_str( char* buf, int size, double num, int width, char justify_right, int prec );
static char*	libMU_double_to_exp_str( char* buf, int size, double num, int width, char justify_right, int prec );
static double	libMU_frexp10( double num, int* exp_out );

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
int	libMU_snprintf( char* buf, unsigned int size, const char* format, ... )
{
    va_list vls;
	if( buf == NULL || size < 1 ) return 0;
	if( format == NULL ) return 0;
    va_start( vls, format );
    int nchar = libMU_vsnprintf(buf, size, format, vls);
    va_end( vls );
    return nchar;
}

/**
 * Low footprint snprintf implementation with OR without floating point support
 * @param	buf		Pointer to destination character array
 * @param	size	Size of destination character array
 * @param	format	printf() format specification
 * @param	vls		Variable length argument variable
 * @return			Number of characters written into buf
 * @see				libMU_snprintf()
 */
int	libMU_vsnprintf( char* buf, unsigned int size, const char* format, va_list vls )
{
    char fill_char; int width, prec; int num_fill, error;
    char* buf_start = buf; char *end = buf + size - 1;	/* Leave space for string end character */
	if( buf == NULL || size < 1 ) return 0;
	if( format == NULL ) return 0;
    for(; format[0] && buf < end; format++ ) {
    	if( format[0] != '%' ) {
    		buf[0] = format[0]; buf++; continue;
    	}
    	/* Start format processing */
    	format++;
    	/* Check for '%' character */
    	if( format[0] == '%' ) {
    		buf[0] = '%'; buf++; continue;
    	}
    	/* Check for fill character */
    	fill_char = 0;
    	if( format[0] == ' ' || format[0] == '0' ) {
    		fill_char = format[0]; format++;
    		while( format[0] == fill_char ) format++;
    	}
    	/* Get width */
    	width = -1;
    	if( format[0] >= '1' && format[0] <= '9' ) {
    		width = format[0] - '0'; format++;
    		while( format[0] >= '0' && format[0] <= '9' ) {
    			width *= 10; width += ( format[0] - '0' ); format++;
    		}
    	}
		/* Get precision */
		if( format[0] == '.' ) {
			format++; prec = 0;
    		while( format[0] >= '0' && format[0] <= '9' ) {
    			prec *= 10; prec += ( format[0] - '0' ); format++;
    		}
		}else{
			prec = -1;
		}
    	/* Process format */
		error = 0;
    	switch( format[0] ) {
    	case 's':
    		{
				char* str = va_arg( vls, char* );
				int len = strlen( str );
				if( width > 0 && ( prec == -1 || prec > width ) ) prec = width;
				if( prec >= 0 && len > prec ) len = prec;
				if( fill_char && width > len ) {	/* Right justify */
					num_fill = width - len;
					if( (buf + num_fill) > end ) num_fill = end - buf;
					memset( buf, fill_char, num_fill ); buf += num_fill;
				}
				if( (buf + len) >= end ) len = end - buf;
				memcpy( buf, str, len ); buf += len;
				if( !fill_char && width > len ) {	/* Left justify */
					num_fill = width - len;
					if( (buf + num_fill) > end ) num_fill = end - buf;
					memset( buf, ' ', num_fill ); buf += num_fill;
				}
    		}
    		break;
    	case 'i':
    	case 'd':
    		{
    			int num = va_arg( vls, int );
    			buf = libMU_int_to_str( buf, end-buf, num, width, fill_char );
    		}
    		break;
    	case 'u':
    		{
    			unsigned int num = va_arg( vls, unsigned int );
    			buf = libMU_uint_to_str( buf, end-buf, num, width, fill_char );
    		}
    		break;
    	case 'x':
    	case 'X':
    		{
				unsigned int num = va_arg( vls, unsigned int );
				buf = libMU_uint_to_hex( buf, end-buf, num, width, fill_char );
    		}
    		break;
    	case 'c':
    		{
    			char c = va_arg( vls, char );
    			if( buf < end ) { *buf = c; buf++; }
    		}
    		break;
#if defined(LIBMU_SNPRINTF_USE_FLOAT)
		case 'f':
		case 'F':
			{
				double num = va_arg( vls, double );
				if( prec == -1 ) prec = 6;
				buf = libMU_double_to_str( buf, end-buf, num, width, fill_char, prec );
			}
			break;
		case 'g':
		case 'G':
			{
				double num = va_arg( vls, double );
				if( prec == -1 ) prec = 6;
				buf = libMU_double_to_exp_str( buf, end-buf, num, width, fill_char, prec );
			}
			break;
#endif
    	default:
			error = 1;
			break;
    	}
		if( error ) {
			if( width > 0 ) num_fill = width;
			else 			num_fill = 3;
			if( (buf + num_fill) > end ) num_fill = end - buf;
			memset( buf, LIBMU_SNPRINTF_ERROR_CHAR, num_fill ); buf += num_fill;
			break;
    	}
    }
    buf[0] = 0;
    libMU_vsnprintf_vls_out = vls;
	return ( buf - buf_start );
}

/*
 * ###########################################################################
 * ###########################################################################
 */
static char*	libMU_uint_to_str( char* buf, int size, unsigned int num, int width, char justify_right )
{
	unsigned int dig;
	char* end = buf + size;
	if( buf == NULL || buf >= end ) return buf;
	if( justify_right != 0 ) {
		if( justify_right != ' ' && justify_right != '0' ) justify_right = ' ';
	}
	if( !justify_right || width < 0 ) {
		unsigned int div = 1000000000;		/* implementation dependent (works on 32 bit systems) */
		int first_dig = 0;
		while( buf < end && div > 0 ) {
			dig = num / div; num -= div * dig; div /= 10;
			if( dig > 0 ) first_dig = 1;
			if( first_dig ) {
				buf[0] = dig + '0'; buf++; width--;
			}
		}
		if( !first_dig ) {
			buf[0] = '0'; buf++; width--;
		}
		while( buf < end && width > 0 ) {
			buf[0] = ' '; buf++; width--;
		}
		if( num > 0 ) {
			buf[-1] = LIBMU_SNPRINTF_ERROR_CHAR;	/* Number does not fit in the given size */
		}
		return buf;
	}
	if( width > size ) width = size;
	end = buf + width; buf = end - 1;
	if( num == 0 ) {
		buf[0] = '0'; buf--; width--;
	}else{
		while( width > 0 && num > 0 ) {
			dig = num % 10; num /= 10;
			buf[0] = dig + '0'; buf--; width--;
		}
	}
	while( width > 0 ) {
		buf[0] = justify_right; buf--; width--;
	}
	if( num > 0 ) {
		buf[1] = LIBMU_SNPRINTF_ERROR_CHAR;	/* Number does not fit in the given width or size */
	}
	return end;
}

static char*	libMU_int_to_str( char* buf, int size, int num, int width, char justify_right )
{
	int dig, sign = 0;
	char* end = buf + size;
	if( buf == NULL || buf >= end ) return buf;
	if( num < 0 ) {
		sign = 1; num = -num;
	}
	if( !justify_right || width < 0 ) {
		int div = 1000000000;		/* implementation dependent (works on 32 bit systems) */
		int first_dig = 0;
		if( sign ) {
			buf[0] = '-'; buf++; width--;
		}
		while( buf < end && div > 0 ) {
			dig = num / div; num -= div * dig; div /= 10;
			if( dig > 0 ) first_dig = 1;
			if( first_dig ) {
				buf[0] = dig + '0'; buf++; width--;
			}
		}
		if( !first_dig ) {
			buf[0] = '0'; buf++; width--;
		}
		while( buf < end && width > 0 ) {
			buf[0] = ' '; buf++; width--;
		}
		if( num > 0 ) {
			buf[-1] = LIBMU_SNPRINTF_ERROR_CHAR;	/* Number does not fit in the given size */
		}
		return buf;
	}
	if( width > size ) width = size;
	end = buf + width; buf = end - 1;
	if( num == 0 ) {
		buf[0] = '0'; buf--; width--;
	}else{
		while( width > 0 && num > 0 ) {
			dig = num % 10; num /= 10;
			buf[0] = dig + '0'; buf--; width--;
		}
	}
	if( justify_right == '0' ) {
		while( width > 0 ) {
			buf[0] = '0'; buf--; width--;
		}
		if( sign && buf[1] == '0' ) {
			buf[1] = '-'; sign = 0;
		}
	}else{
		if( width > 0 && sign ) {
				buf[0] = '-'; buf--; width--; sign = 0;
		}
		while( width > 0 ) {
			buf[0] = ' '; buf--; width--;
		}
	}
	if( num > 0 || sign ) {
		buf[1] = LIBMU_SNPRINTF_ERROR_CHAR;	/* Number does not fit in the given width or size */
	}
	return end;
}

static char*	libMU_uint_to_hex( char* buf, int size, unsigned int num, int width, char fill_char )
{
	unsigned int dig;
	char* end = buf + size;
	if( buf == NULL || buf >= end ) return buf;
	if( width < 0 ) width = 8;
	if( !fill_char ) fill_char = '0';
	if( width > size ) width = size;
	end = buf + width; buf = end - 1;
	if( num == 0 ) {
		buf[0] = '0'; buf--; width--;
	}else{
		while( width > 0 && num > 0 ) {
			dig = num & 0xF; num >>= 4;
			if( dig > 9 )	dig += ('A'-10);
			else			dig += '0';
			buf[0] = dig; buf--; width--;
		}
	}
	while( width > 0 ) {
		buf[0] = fill_char; buf--; width--;
	}
	return end;
}

#if defined(LIBMU_SNPRINTF_USE_FLOAT)
static char*	libMU_double_to_str( char* buf, int size, double num, int width, char justify_right, int prec )
{
	int dig, sign = 0, i;
	char* end = buf + size;
	uint64_t num_part;
	int first_dig = 0;
	if( buf == NULL || buf >= end ) return buf;
	if( num < 0 ) {
		sign = 1; num = -num;
	}
	if( !justify_right || width < 0 ) {
		uint64_t div = 10000000000000000LL;		/* implementation dependent (works on 64 bit integers) */
		char* buf_nonzero;
		double num_round;
#if defined(DEBUG)
		ASSERT( (div/100000000) == 100000000 );	/* Check 64 bit support */
#endif
		if( width < 0 || width > size ) width = size;
		if( sign ) {
			buf[0] = '-'; buf++; width--;
		}
		/* See if integer part has more than 16 digits */
		if( num >= 1e17 && width > 0 ) {
			uint64_t rest = (uint64_t)(num*1e-17);
			num -= 1e17*rest;
			buf[0] = LIBMU_SNPRINTF_ERROR_CHAR;	/* Number does not fit in the given size */
			buf++; width--;
		}
		/* integer part  */
		num_part = (uint64_t)num;
		if( num_part == 0 ) {
			if( width > 0 ) {
				buf[0] = '0'; buf++; width--;
			}
			div = 0;
		}else{
			while( width > 0 && div > 0 ) {
				dig = (int)(num_part / div); num_part -= div * dig; div /= 10;
				if( dig > 0 ) first_dig = 1;
				if( first_dig ) {
					buf[0] = dig + '0'; buf++; width--;
				}
			}
		}
		if( div > 0 ) {
			buf[-1] = LIBMU_SNPRINTF_ERROR_CHAR;	/* Number does not fit in the given size */
			return buf;
		}
		/* Calculate fractional part */
		num_part = (uint64_t)num;
		num -= num_part;
		if( num == 0 ) return buf;
		buf_nonzero = buf;
		if( width > 0 ) {
			buf[0] = '.'; buf++; width--;
		}
		/* Manage rounding of digits */
		if( prec == -1 || prec > width ) prec = width;
		num_round = 0.5;
		for( i = 0; i < prec; i++ ) num_round /= 10;
		num += num_round;
		/* Convert fractional digits */
		while( width > 0 && num > 0 && prec != 0 ) {
			num *= 10; dig = (int)num; num -= dig;
			buf[0] = dig + '0'; buf++; width--; prec--;
			if( dig > 0 ) buf_nonzero = buf;
		}
		return buf_nonzero;
	}
	if( width > size ) width = size;
	end = buf + width; buf = end - 1;
	if( num == 0 ) {
		buf[0] = '0'; buf--; width--; num_part = 0;
	}else{
		/* Manage precision */
		if( prec == -1 || (prec + 2) > width ) prec = width - 2;
		for( i = 0; i < prec; i++ ) num *= 10;
		num += 0.5;	/* round */
		num_part = (uint64_t)num;
		/* convert digits */
		while( width > 0 && ( num_part > 0 || prec >= 0 ) ) {
			dig = num_part % 10; num_part /= 10;
			buf[0] = dig + '0'; buf--; width--; prec--;
			if( prec == 0 /*&& width > 0*/ ) { /* Previously managed */
				buf[0] = '.'; buf--; width--; 
			}
		}
	}
	if( width > 0 && sign ) {
		buf[0] = '-'; buf--; width--; sign = 0;
	}
	while( width > 0 ) {
		buf[0] = ' '; buf--; width--;
	}
	if( num_part > 0 || sign ) {
		buf[1] = LIBMU_SNPRINTF_ERROR_CHAR;	/* Number does not fit in the given width or size */
	}
	return end;
}

#if defined(LIBMU_SNPRINTF_USE_LOG)
#include <math.h>
#endif

static double libMU_frexp10( double num, int* exp_out )
{
	int exp = 0, exp_prev, sign = 0; 
	double factor = 1; double factor_prev;
	if( num == 0 ) {
		if( exp_out != NULL ) *exp_out = 0;
		return 0;
	}
	if( num < 0 ) { sign = 1; num = -num; }
#if !defined(LIBMU_SNPRINTF_USE_LOG)
	if( num >= 1 ) {
		while( num > factor ) {
			factor_prev = factor; exp_prev = exp;
			factor *= 1e100; exp += 100;
		}
		factor = factor_prev; exp = exp_prev;
		while( num > factor ) {
			factor_prev = factor; exp_prev = exp;
			factor *= 1e10; exp += 10;
		}
		factor = factor_prev; exp = exp_prev;
		while( num > factor ) {
			factor_prev = factor; exp_prev = exp;
			factor *= 1e1; exp++;
		}
		factor = factor_prev; exp = exp_prev;
	}else{
		while( num < factor ) {
			factor_prev = factor; exp_prev = exp;
			factor *= 1e-100; exp-=100;
		}
		factor = factor_prev; exp = exp_prev;
		while( num < factor ) {
			factor_prev = factor; exp_prev = exp;
			factor *= 1e-10; exp-=10;
		}
		factor = factor_prev; exp = exp_prev;
		while( num < factor ) {
			factor_prev = factor; exp_prev = exp;
			factor *= 1e-1; exp--;
		}
	}
	/* normalize number to d.ddddd... */
	num /= factor;
#else
	if( num > 1 )	exp = log10(num);
	else			exp = log10(num) - 0.999999999999;
	num *= pow( 10.0, -exp );
#endif
	if( exp_out != NULL ) *exp_out = exp;
	if( sign ) return -num;
	return num;
}

static char*	libMU_double_to_exp_str( char* buf, int size, double num, int width, char justify_right, int prec )
{
	double num_norm; int exp = 0, chars, num_fill;
	char* end = buf+size;
	num_norm = libMU_frexp10( num, &exp );
	if( exp <= 15 && exp >= -6 ) {
		if( prec > 0 ) {
			prec -= exp+1;
			if( prec < 0 ) prec = 0;
		}
		return libMU_double_to_str( buf, size, num, width, justify_right, prec );
	}
	if(      exp >= 100 )	chars = 5;
	else if( exp >=  10 )	chars = 4;
	else if( exp >=   0 )	chars = 4;
	else if( exp <=-100 )	chars = 5;
	else if( exp <= -10 )	chars = 4;
	else 					chars = 4;
	if( size <= chars || ( width > 0 && width <= chars ) ) {
		num_fill = 3;
		if( (buf + num_fill) > end ) num_fill = end - buf;
		memset( buf, LIBMU_SNPRINTF_ERROR_CHAR, num_fill ); buf += num_fill;
		return buf;
	}
	if( width > 0 ) {
		char* buf_save;
		buf_save = libMU_double_to_str( buf, end-buf, num_norm, width-chars, 0, prec );
		if( justify_right ) {	/* Right aligned */
			buf = buf_save;
			buf[0] = 'e'; buf++;
			if( exp < 0 )	{ buf[0] = '-'; buf++; exp = -exp; }
			else			{ buf[0] = '+'; buf++; }
			return libMU_uint_to_str( buf, end-buf, exp, chars-2, '0' );
		}else{				/* Left aligned */
			/* Search place for exponential part */
			memset( buf_save, ' ', chars ); buf_save += chars;
			buf = buf_save;
			while( buf[-1] == ' ' ) buf--;
			buf[0] = 'e'; buf++;
			if( exp < 0 )	{ buf[0] = '-'; buf++; exp = -exp; }
			else			{ buf[0] = '+'; buf++; }
			libMU_uint_to_str( buf, end-buf, exp, chars-2, '0' );
			return buf_save;
		}
	}
	/* Minimum size */
	buf = libMU_double_to_str( buf, end-buf, num_norm, width, justify_right, prec );
	if( (end-buf) < chars ) {
		buf = end - chars - 1;
		buf[0] = LIBMU_SNPRINTF_ERROR_CHAR; buf++;
	}
	buf[0] = 'e'; buf++;
	if( exp < 0 )	{ buf[0] = '-'; buf++; exp = -exp; }
	else			{ buf[0] = '+'; buf++; }
	return libMU_uint_to_str( buf, end-buf, exp, chars-2, '0' );
}
#endif/*LIBMU_SNPRINTF_USE_FLOAT */

/**
 * convert string to int32_t
 * @param	buf		Pointer to character array to convert
 * @return			Converted number
 * @note			Skips all initial whitespace
 * 					Converts until a nondigit character is found
 * 					(other than the initial '-' sign)
 */
int32_t libMU_atoi( char* buf )
{
	int32_t num = 0, sign = 0;
	while( buf[0] == ' ' || buf[0] =='\t' ) buf++;	/* Skip whitespace */
	if( buf[0] == '-' ) { sign = 1; buf++; }
	while( buf[0] >= '0' && buf[0] <= '9' ) {
		num *= 10; num += ( buf[0] - '0' ); buf++;
	}
	if( sign ) return -num;
	return num;
}

/**
 * convert string to uint32_t
 * @param	buf		Pointer to character array to convert
 * @return			Converted number
 * @note			Skips all initial whitespace
 * 					Converts until a nondigit character is found
 */
uint32_t libMU_atoul( char* buf )
{
	uint32_t num = 0;
	while( buf[0] == ' ' || buf[0] =='\t' ) buf++;	/* Skip whitespace */
	while( buf[0] >= '0' && buf[0] <= '9' ) {
		num *= 10; num += ( buf[0] - '0' ); buf++;
	}
	return num;
}

/**
 * convert string to int64_t
 * @param	buf		Pointer to character array to convert
 * @return			Converted number
 * @note			Skips all initial whitespace
 * 					Converts until a nondigit character is found
 * 					(other than the initial '-' sign)
 */
int64_t libMU_atoll( char* buf )
{
	int64_t num = 0, sign = 0;
	while( buf[0] == ' ' || buf[0] =='\t' ) buf++;	/* Skip whitespace */
	if( buf[0] == '-' ) { sign = 1; buf++; }
	while( buf[0] >= '0' && buf[0] <= '9' ) {
		num *= 10; num += ( buf[0] - '0' ); buf++;
	}
	if( sign ) return -num;
	return num;
}

/**
 * convert string to uint64_t
 * @param	buf		Pointer to character array to convert
 * @return			Converted number
 * @note			Skips all initial whitespace
 * 					Converts until a nondigit character is found
 */
uint64_t libMU_atoull( char* buf )
{
	uint64_t num = 0;
	while( buf[0] == ' ' || buf[0] =='\t' ) buf++;	/* Skip whitespace */
	while( buf[0] >= '0' && buf[0] <= '9' ) {
		num *= 10; num += ( buf[0] - '0' ); buf++;
	}
	return num;
}

/**
 * @}
 */
