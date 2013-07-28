/**
 * @addtogroup	libMU_UTF
 * Library for Unicode text conversion and manipulation (UTF8, UTF16, UTF32)
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2013/4/17
 * @copyright	BSDL
 ********************************************************************
 */
#include <libMU/utf.h>

/**
 * Get length of a UTF8 char
 * @param txt	Pointer to character
 * @return 		UTF8 Character length: 1-4 if correct, 0 if invalid char
 * @note		Restricted by RFC3629 to conform to UTF16 limits
 */
int utf8_get_length( const uint8_t* txt )
{
	if( txt[0] < 0x80 ) return 1;
	if( txt[0] < 0xC0 ) return 0;/* continuation character */
	if( txt[0] < 0xE0 ) return 2;
	if( txt[0] < 0xF0 ) return 3;
	if( txt[0] < 0xF8 ) return 4;
	/* Restricted by RFC3629 to conform to UTF16 limits */
#if 0
	if( txt[0] < 0xFC ) return 5;
	if( txt[0] < 0xFE ) return 6;

	if( (txt[0] & 0x80) == 0x00 ) return 1;
	if( (txt[0] & 0xE0) == 0xC0 ) return 2;
	if( (txt[0] & 0xF0) == 0xE0 ) return 3;
	if( (txt[0] & 0xF8) == 0xF0 ) return 4;
	/* Restricted by RFC3629 to conform to UTF16 limits */
	if( (txt[0] & 0xFC) == 0xF8 ) return 5;
	if( (txt[0] & 0xFE) == 0xFC ) return 6;
#endif
	return 0;
}

/**
 * Check if a UTF8 char of given length is valid
 * @param txt	Pointer to character
 * @param len	Length of character
 * @return 		UTF8 Character length: 1-4 if correct, 0 if invalid char
 * @note		Restricted by RFC3629 to conform to UTF16 limits
 */
int utf8_check_char( const uint8_t* txt, uint32_t len )
{
	switch( len ) {
	case 1: return 1;
	case 2:	if( (txt[1] & 0xC0) != 0x80 ) return 2;
			return 0;
	case 3:	if( (txt[1] & 0xC0) == 0x80 && (txt[2] & 0xC0) == 0x80 ) return 3;
			return 0;
	case 4:	if( (txt[1] & 0xC0) == 0x80 && (txt[2] & 0xC0) == 0x80 && 
				(txt[3] & 0xC0) == 0x80 ) return 4;
			return 0;
#if 0
	/* Restricted by RFC3629 to conform to UTF16 limits */
	case 5:	if( (txt[1] & 0xC0) == 0x80 && (txt[2] & 0xC0) == 0x80 &&
				(txt[3] & 0xC0) == 0x80 && (txt[4] & 0xC0) == 0x80 ) return 5;
			return 0;
	case 6:	if( (txt[1] & 0xC0) == 0x80 && (txt[2] & 0xC0) == 0x80 &&
				(txt[3] & 0xC0) == 0x80 && (txt[4] & 0xC0) == 0x80 &&
				(txt[5] & 0xC0) == 0x80 ) return 6;
			return 0;
#endif
	}
	return 0;
}

/**
 * Check a buffer to verify it is valid UTF8 data
 * @param txt	Pointer to data
 * @param len	Length of data
 * @return 		0 if invalid data, number of bytes of valid data otherwise
 * @note		Restricted by RFC3629 to conform to UTF16 limits
 * 				A 0 value will end the check regardless the length of the data,
 * 				This may happen if we have received a partial buffer
 */
int utf8_check_buffer( const uint8_t* txt, uint32_t len )
{
	uint32_t char_len = 1; const uint8_t* end = txt + len;
	while( char_len > 0 && txt < end && *txt ) {
		char_len = utf8_get_length( txt );
		if( (txt + char_len) > end ) break;
		char_len = utf8_check_char( txt, char_len );
		txt += char_len;
	}
	if( char_len == 0 ) return 0;
	return ( len - (end - txt) );
}

/**
 * Check a 0 terminated string to verify it is valid UTF8 data
 * @param txt	Pointer to 0 terminated string
 * @return 		0 if invalid data, nonzero otherwise
 * @note		Restricted by RFC3629 to conform to UTF16 limits
 */
int utf8_check_string( const uint8_t* txt )
{
	uint32_t char_len = 1;
	while( char_len > 0 && *txt ) {
		char_len = utf8_check_char( txt, utf8_get_length( txt ) );
		txt += char_len;
	}
	if( char_len == 0 ) return 0;
	return 1;
}

/**
 * Convert one UTF8 character to UTF32
 * @param txt	Pointer to character
 * @param len	Pointer to variable to hold the length of the converted character
 * @return 		UTF32 representation of the converted character
 */
uint32_t utf8_to_utf32_char( const uint8_t* txt, unsigned int* len )
{
	*len = 0;
	if( (txt[0] & 0x80) == 0 ) {
		*len = 1;
		return txt[0];
	}
	if( (txt[0] & 0xE0) == 0xC0 ) {
		uint32_t t0 = txt[0], t1 = txt[1];
		if( (t1 & 0xC0) != 0x80 ) return (uint32_t)(-1);
		*len = 2;
		return ( (t0 &~0xE0)<< 6 | (t1 &~0xC0) );
	}
	if( (txt[0] & 0xF0) == 0xE0 ) {
		uint32_t t0 = txt[0], t1 = txt[1], t2 = txt[2];
		if( (t1 & 0xC0) != 0x80 || (t2 & 0xC0) != 0x80 ) return (uint32_t)(-1);
		*len = 3;
		return ( (t0 &~0xF0)<<12 | (t1 &~0xC0)<< 6 | (t2 &~0xC0) );
	}
	if( (txt[0] & 0xF8) == 0xF0 ) {
		uint32_t t0 = txt[0], t1 = txt[1], t2 = txt[2], t3 = txt[3];
		if( (t1 & 0xC0) != 0x80 || (t2 & 0xC0) != 0x80 ||
			(t3 & 0xC0) != 0x80 ) return (uint32_t)(-1);
		*len = 4;
		return ( (t0 &~0xF8)<<18 | (t1 &~0xC0)<<12 | (t2 &~0xC0)<< 6 | (t3 &~0xC0) );
	}
	/* Restricted by RFC3629 to conform to UTF16 limits */
	if( (txt[0] & 0xFC) == 0xF8 ) {
		uint32_t t0 = txt[0], t1 = txt[1], t2 = txt[2], t3 = txt[3], t4 = txt[4];
		if( (t1 & 0xC0) != 0x80 || (t2 & 0xC0) != 0x80 ||
			(t3 & 0xC0) != 0x80 || (t4 & 0xC0) != 0x80 ) return (uint32_t)(-1);
		*len = 5;
		return ( (t0 &~0xFC)<<24 | (t1 &~0xC0)<<18 | (t2 &~0xC0)<<12 | (t3 &~0xC0)<< 6 | (t4 &~0xC0) );
	}
	if( (txt[0] & 0xFE) == 0xFC ) {
		uint32_t t0 = txt[0], t1 = txt[1], t2 = txt[2], t3 = txt[3], t4 = txt[4], t5 = txt[5];
		if( (t1 & 0xC0) != 0x80 || (t2 & 0xC0) != 0x80 ||
			(t3 & 0xC0) != 0x80 || (t4 & 0xC0) != 0x80 ||
			(t5 & 0xC0) != 0x80 ) return (uint32_t)(-1);
		*len = 6;
		return ( (t0 &~0xFE)<<30 | (t1 &~0xC0)<<24 | (t2 &~0xC0)<<18 |
				 (t3 &~0xC0)<<12 | (t4 &~0xC0)<< 6 | (t5 &~0xC0) );
	}
	return (uint32_t)(-1);
}

/**
 * Convert one UTF16 character to UTF32
 * @param txt	Pointer to character
 * @param len	Pointer to variable to hold the length of the converted character
 * @return 		UTF32 representation of the converted character
 */
uint32_t utf16_to_utf32_char( const uint16_t* txt, unsigned int* len )
{
	if( (txt[0] & 0xDC00) == 0xD800 && (txt[1] & 0xDC00) == 0xDC00 ) {
		uint32_t uc = ( (txt[0] & 0x3FF) << 10 ) | (txt[1] & 0x3FF);
		*len = 2;
		return (uc + 0x10000);
	}
	*len = 1;
	return (uint32_t)txt[0];
}

/**
 * Convert one UTF32 char to UTF16
 * @param uchar	Codepoint of UTF32 character
 * @param extra	Pointer to variable to hold the extra data of the UTF16 encoding of the character
 * @return 		UTF16 representation of the converted character
 */
uint16_t utf32_to_utf16_chars( uint32_t uchar, uint16_t* extra )
{
	if( uchar <= 0xFFFF ) {
		*extra = 0; return (uint16_t)uchar;
	}
	if( uchar > 0x10FFFF ) {
		*extra = UINT16_MAX; return 0;
	}
	/* v  = 0x64321
	 * v′ = v - 0x10000 = 0x54321 = 0101 0100 0011 0010 0001 */
	uchar -= 0x10000;   
	/* vh = v′ >> 10   = 01 0101 0000 // higher 10 bits of v′
	 * vl = v′ & 0x3FF = 11 0010 0001 // lower  10 bits of v′
	 * w2 = 0xDC00 + vl = 1101 1111 0010 0001 = 0xDF21 // second code unit of UTF-16 encoding */
	*extra = 0xDC00 + (uchar & 0x3FF);
	/* w1 = 0xD800 + vh = 1101 1001 0101 0000 = 0xD950 // first code unit of UTF-16 encoding */
	return (0xD800 + ((uchar>>10) & 0x3FF));
}

/**
 * Convert UTF8 string to UTF16
 * @param utf16_string	Pointer to array where the converted string will be stored
 * 						(May be NULL if we only want to count the number of
 * @param max_chars		Size of the array where the converted string will be stored
 * @param utf8_string	Pointer to UTF8 encoded string
 * @return 				Number of UTF16 chars in the converted string
 * @note
 * If utf16_string is NULL this function may be used to calculate the memory
 * size needed to store the converted string
 */
int utf8_to_utf16_string( uint16_t* utf16_string, uint32_t max_chars, const uint8_t* utf8_string )
{
	uint32_t num_chars = 0, len = 1, uc; uint16_t w1,w2;
	max_chars--;	/* Leave space for terminating 0 */
	while( *utf8_string ) {
		/* Read UTF8 char and check if it is correct */
		uc = utf8_to_utf32_char( utf8_string, &len );
		if( len == 0 || uc > 0x10FFFF ) break;
		utf8_string += len;
		/* Store result or count UTF16 chars */
		if( utf16_string == NULL ) {
			num_chars +=  1 + ( uc > 0xFFFF );
		}else{
			/* Convert to UTF16 */
			w1 = utf32_to_utf16_chars( uc, &w2 );
			if( w2 && (num_chars + 1) >= max_chars ) break;
			*utf16_string++ = w1; num_chars++;
			if( w2 ) {
				*utf16_string++ = w2; num_chars++;
			}
		}
	}
	if( utf16_string ) *utf16_string = 0;
	return num_chars;
}

/**
 * Convert UTF8 string to UTF32
 * @param utf32_string	Pointer to array where the converted string will be stored
 * 						(May be NULL if we only want to count the number of
 * @param max_chars		Size of the array where the converted string will be stored
 * @param utf8_string	Pointer to UTF8 encoded string
 * @return 				Number of UTF32 chars in the converted string
 * @note
 * If utf32_string is NULL this function may be used to calculate the memory
 * size needed to store the converted string
 */
int utf8_to_utf32_string( uint32_t* utf32_string, uint32_t max_chars, const uint8_t* utf8_string )
{
	uint32_t num_chars = 0, len = 1, uc;
	max_chars--;	/* Leave space for terminating 0 */
	while( *utf8_string ) {
		/* Read UTF8 char and check if it is correct */
		uc = utf8_to_utf32_char( utf8_string, &len );
		if( len == 0 ) break;
		utf8_string += len;
		/* Store result or count UTF16 chars */
		if( utf32_string == NULL ) {
			num_chars++;
		}else{
			if( num_chars >= max_chars ) break;
			*utf32_string++ = uc; num_chars++;
		}
	}
	if( utf32_string ) *utf32_string = 0;
	return num_chars;
}

/**
 * Convert UTF16 string to UTF8
 * @param utf8_string	Pointer to array where the converted string will be stored
 * 						(May be NULL if we only want to count the number of
 * @param max_chars		Size of the array where the converted string will be stored
 * @param utf16_string	Pointer to UTF16 encoded string
 * @return 				Number of UTF8 chars in the converted string
 * @note
 * If utf8_string is NULL this function may be used to calculate the memory
 * size needed to store the converted string
 */
int utf16_to_utf8_string( uint8_t* utf8_string, uint32_t max_chars, const uint16_t* utf16_string )
{
	uint32_t num_chars = 0, len = 1, uc; uint8_t mask;
	max_chars--;	/* Leave space for terminating 0 */
	while( *utf16_string ) {
		/* Read UTF16 char and check if it is correct */
		uc = utf16_to_utf32_char( utf16_string, &len );
		if( len == 0 ) break;
		utf16_string += len;
		/* UTF8 char length (the maximum character length will be 4 when converting from UTF16) */
		len = 1 + ( uc > 0x7F ) + ( uc > 0x3FF ) + ( uc > 0xFFFF ) + ( uc > 0x1FFFFF ) + ( uc > 0x3FFFFFF );
		/* Store result or count UTF16 chars */
		if( utf8_string == NULL ) {
			num_chars += len;
		}else{
			/* check if we have enough space for char */
			if( (num_chars + len) > max_chars ) break;
			/* Convert to UTF8 */
			switch( len ) {
			case 6: utf8_string[5] = 0x80 | (uc & 0x3F); uc >>= 6; /* mask = 0xFC */
			case 5: utf8_string[4] = 0x80 | (uc & 0x3F); uc >>= 6; /* mask = 0xF8 */
			case 4: utf8_string[3] = 0x80 | (uc & 0x3F); uc >>= 6; /* mask = 0xF0 */
			case 3: utf8_string[2] = 0x80 | (uc & 0x3F); uc >>= 6; /* mask = 0xE0 */
			case 2: utf8_string[1] = 0x80 | (uc & 0x3F); uc >>= 6; /* mask = 0xC0 */
			case 1: if( len > 1 ) {
						mask = (uint8_t)(0x3F00 >> len);
						utf8_string[0] = mask | (uc &~mask);
					}else{
						utf8_string[0] = (uint8_t)uc;
					}
					break;
			}
			utf8_string += len;
			num_chars += len;
		}
	}
	if( utf8_string ) *utf8_string = 0;
	return num_chars;
}

/**
 * Convert UTF32 string to UTF8
 * @param utf8_string	Pointer to array where the converted string will be stored
 * 						(May be NULL if we only want to count the number of
 * @param max_chars		Size of the array where the converted string will be stored
 * @param utf32_string	Pointer to UTF32 encoded string
 * @return 				Number of UTF8 chars in the converted string
 * @note
 * If utf8_string is NULL this function may be used to calculate the memory
 * size needed to store the converted string
 */
int utf32_to_utf8_string( uint8_t* utf8_string, uint32_t max_chars, const uint32_t* utf32_string )
{
	uint32_t num_chars = 0, len = 1, uc; uint8_t mask;
	max_chars--;	/* Leave space for terminating 0 */
	while( *utf32_string ) {
		/* Read UTF16 char and check if it is correct */
		uc = *utf32_string++;
		/* UTF8 char length (the maximum character length will be 4 when converting from UTF16) */
		len = 1 + ( uc > 0x7F ) + ( uc > 0x3FF ) + ( uc > 0xFFFF ) + ( uc > 0x1FFFFF ) + ( uc > 0x3FFFFFF );
		/* Store result or count UTF16 chars */
		if( utf8_string == NULL ) {
			num_chars += len;
		}else{
			/* check if we have enough space for char */
			if( (num_chars + len) > max_chars ) break;
			/* Convert to UTF8 */
			switch( len ) {
			case 6: utf8_string[5] = 0x80 | (uc & 0x3F); uc >>= 6; /* mask = 0xFC */
			case 5: utf8_string[4] = 0x80 | (uc & 0x3F); uc >>= 6; /* mask = 0xF8 */
			case 4: utf8_string[3] = 0x80 | (uc & 0x3F); uc >>= 6; /* mask = 0xF0 */
			case 3: utf8_string[2] = 0x80 | (uc & 0x3F); uc >>= 6; /* mask = 0xE0 */
			case 2: utf8_string[1] = 0x80 | (uc & 0x3F); uc >>= 6; /* mask = 0xC0 */
			case 1: if( len > 1 ) {
						mask = (uint8_t)(0x3F00 >> len);
						utf8_string[0] = mask | (uc &~mask);
					}else{
						utf8_string[0] = (uint8_t)uc;
					}
					break;
			default:break;
			}
			utf8_string += len;
			num_chars += len;
		}
	}
	if( utf8_string ) *utf8_string = 0;
	return num_chars;
}
/**
 * @}
 */
