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
#ifndef LIBMU_UTF_H_
#define LIBMU_UTF_H_
#include <stdint.h>
#include <wchar.h>

#ifdef  __cplusplus
extern "C" {
#endif

//#if defined(_WIN32) || sizeof(wchar_t) == 2
#if WCHAR_MAX < 0x10000
#define WCHAR_IS_UTF16
#else
#define	WCHAR_IS_UTF32
#endif

/**
 * Get length of a UTF8 char
 * @param txt	Pointer to character
 * @return 		UTF8 Character length: 1-4 if correct, 0 if invalid char
 * @note		Restricted by RFC3629 to conform to UTF16 limits
 */
int utf8_get_length( const uint8_t* txt );

/**
 * Check a buffer to verify it is valid UTF8 data
 * @param txt	Pointer to data
 * @param len	Length of data
 * @return 		0 if invalid data, number of bytes of valid data otherwise
 * @note		Restricted by RFC3629 to conform to UTF16 limits
 * 				A 0 value will end the check regardless the length of the data,
 * 				This may happen if we have received a partial buffer
 */
int utf8_check_buffer( const uint8_t* txt, uint32_t len );

/**
 * Check a 0 terminated string to verify it is valid UTF8 data
 * @param txt	Pointer to 0 terminated string
 * @return 		0 if invalid data, nonzero otherwise
 * @note		Restricted by RFC3629 to conform to UTF16 limits
 */
int utf8_check_string( const uint8_t* txt );

/**
 * Convert one UTF8 character to UTF32
 * @param txt	Pointer to character
 * @param len	Pointer to variable to hold the length of the converted character
 * @return 		UTF32 representation of the converted character
 */
uint32_t utf8_to_utf32_char( const uint8_t* txt, unsigned int* len );

/**
 * Convert one UTF16 character to UTF32
 * @param txt	Pointer to character
 * @param len	Pointer to variable to hold the length of the converted character
 * @return 		UTF32 representation of the converted character
 */
uint32_t utf16_to_utf32_char( const uint16_t* txt, unsigned int* len );

/**
 * Convert one UTF32 char to UTF16
 * @param uchar	Codepoint of UTF32 character
 * @param extra	Pointer to variable to hold the extra data of the UTF16 encoding of the character
 * @return 		UTF16 representation of the converted character
 */
uint16_t utf32_to_utf16_chars( uint32_t uchar, uint16_t* extra );

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
int utf8_to_utf16_string( uint16_t* utf16_string, uint32_t max_chars, const uint8_t* utf8_string );

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
int utf8_to_utf32_string( uint32_t* utf32_string, uint32_t max_chars, const uint8_t* utf8_string );

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
int utf16_to_utf8_string( uint8_t* utf8_string, uint32_t max_chars, const uint16_t* utf16_string );

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
int utf32_to_utf8_string( uint8_t* utf8_string, uint32_t max_chars, const uint32_t* utf32_string );

/**
 * Convert UTF8 string to wchar_t
 * @param wstring		Pointer to array where the converted string will be stored
 * 						(May be NULL if we only want to count the number of
 * @param max_chars		Size of the array where the converted string will be stored
 * @param utf8_string	Pointer to UTF8 encoded string
 * @return 				Number of UTF32 chars in the converted string
 * @note
 * If wstring is NULL this function may be used to calculate the memory
 * size needed to store the converted string
 */
static __inline int utf8_to_wstring( wchar_t* wstring, uint32_t max_chars, const uint8_t* utf8_string )
{
#ifdef WCHAR_IS_UTF16
	return utf8_to_utf16_string( (uint16_t*)wstring, max_chars, utf8_string );
#else
	return utf8_to_utf32_string( (uint32_t*)wstring, max_chars, utf8_string );
#endif
}

/**
 * Convert wchar_t string to UTF8
 * @param utf8_string	Pointer to array where the converted string will be stored
 * 						(May be NULL if we only want to count the number of
 * @param max_chars		Size of the array where the converted string will be stored
 * @param wstring		Pointer to wchar_t encoded string
 * @return 				Number of UTF8 chars in the converted string
 * @note
 * If utf8_string is NULL this function may be used to calculate the memory
 * size needed to store the converted string
 */
static __inline int wstring_to_utf8_string( uint8_t* utf8_string, uint32_t max_chars, const wchar_t* wstring )
{
#ifdef WCHAR_IS_UTF16
	return utf16_to_utf8_string( utf8_string, max_chars, (const uint16_t*)wstring );
#else
	return utf32_to_utf8_string( utf8_string, max_chars, (const uint32_t*)wstring );
#endif
}

#ifdef  __cplusplus
}
#endif

#endif/*LIBMU_UTF_H_*/
/**
 * @}
 */
