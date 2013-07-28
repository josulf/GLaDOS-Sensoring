/**
 * @addtogroup	libMU_Internet
 * Library for internet access
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 * @note
 * Adding new protocols is not trivial (based on uIP library)
 * The supported protocols are:
 * - DHCP (only when reset, there is no reconnection procedure)
 * - DNS (maximum of 10 addresses are cached)
 * - UDP (maximum of 3 connections)
 * - TCP (maximum of 3 connections)
 * - HTTP client
 * - HTTP server
 * - Minimal telnet client (partial support)
 * - Siemens TCP/IP protocol
 */

#ifndef INTERNET_H_
#define INTERNET_H_

#include <stdint.h>
#include <lib/network_state.h>

/**
 * Constants used in the Internet communications
 * @note
 * - Additional constants are defined in network_state.h
 */
enum libMU_Internet_Constants {
	NETWORK_MAX_TAGS = 10	/* Maximum number of web pages served by the HTTP server */
};

/**
 * Data type to store IPv4 addresses
 */
typedef uint32_t	IPAddress_t;

/**
 * Data type for a callback function to process data downloaded from the Internet
 * @param data		Pointer to downloaded data
 * @param len		Size of downloaded data
 * @note
 * - Due to memory limitations, the page is not saved and it has to be processed
 *   as data arrives.
 * - This function will be called several times for each page except for those
 *   pages that are smaller than 1kB (aprox.).
 * @see libMU_Internet_GetPage()
 */
typedef void (*libMU_Internet_PageReaderFunction_t)( char *data, unsigned short int len );

/**
 * Data type for a callback function to process form data received with the HTTP methods GET and POST
 * @param cmd_param	GET/POST method form data pointer
 * @param len		Pointer to save size of the response data
 * @return			Pointer to response data to be sent back by the Web server
 * @note
 * - If a NULL value is returned the server does not send anything back
 * @see libMU_Internet_Server_SetCommandProcessingInfo()
 */
typedef char* (*libMU_Internet_CommandProcessingFunction_t)( const char *cmd_param, int* len );

/**
 * Data type for FreeRTOS task functions
 * @param param		Pointer to generic data
 */
typedef void (*libMU_Task_t)(void *param);

/**
 * Initialize the Internet communication interface.
 * @note
 * - FreeRTOS is used to create and manage tasks
 *   > The Ethernet interrupt handler is setup to receive packets
 *   > The TCP/IP processing task is created
 * - The FreeRTOS system must be started with
 *   vTaskStartScheduler() or libMU_Internet_Start()
 */
void libMU_Internet_Initialize(void);

/**
 * Start Internet protocol manager (start the FreeRTOS scheduler)
 * @note
 * - This function should never return. The only occasion when this
 *   happens is when there is not enough memory to start FreeRTOS
 * - This function MUST be called after the the libMU_Internet_Initalize()
 * @see libMU_Internet_Initialize
 */
void libMU_Internet_Start(void);

/**
 * Create a helper task where the processing will continue
 * @param	task	Pointer to task function
 * @note
 * - This function should be called only once.
 * @see libMU_Internet_Initialize, libMU_Internet_Start
 */
void libMU_Internet_CreateTask(libMU_Task_t task);

/**
 * @return	Connection status
 * @note
 * Tha returned value is one of the following:
 * NETWORK_IP_MISSING, NETWORK_IP_OK, NETWORK_ERROR
 */
int libMU_Internet_GetStatus(void);

/**
 * @return	Local device IP address
 */
IPAddress_t libMU_Internet_GetDeviceIP(void);

/**
 * @param	time_units	Time to wait in ms
 */
void libMU_Internet_Delay(unsigned long time_units);

/**
 * @return	Number of IP frames received
 */
int libMU_Internet_GetNumberOfFramesReceived(void);

/**
 * @return	Number of IP frames sent
 */
int libMU_Internet_GetNumberOfFramesSent(void);

/**
 * Get an IP address of an Internet node using DNS
 * @param host			Device name whose IP address is desired
 * @param ip			Where to store the resolved IP address
 * @param timeout_ms	Maximum timeout in ms
 * @return				true if successfully resolved.
 * @note
 * This implementation allows hostnames of 31 characters max.
 */
int	libMU_Internet_DNS_resolution(const char *host, IPAddress_t* ip, int timeout_ms );

/**
 * Reads a page from the Internet and defines a callback function to process the page
 * @param url		Web page URL address
 * @param function	Page reader function
 * @return			true if connection successfully set up
 * @note
 * This function can not read the full page because of memory limitations
 * @see libMU_Internet_PageReaderFunction_t
 */
int	libMU_Internet_GetPage( char *url, libMU_Internet_PageReaderFunction_t funcion );

/**
 * @return true if it has finished reading the page (or an error has happened)
 * @note
 * See libMU_Internet_GetPage()
 */
int	libMU_Internet_IsPageReadingFinished( void );

/**
 * Initialize IPv4 address from its partial numbers
 * @param ip	Pointer where the IPv4 address is stored
 * @param ip1	1st octet of IPv4 address
 * @param ip2	2nd octet of IPv4 address
 * @param ip3	3rd octet of IPv4 address
 * @param ip4	4th octet of IPv4 address
 */
static __inline void libMU_IP_Initialize( IPAddress_t* ip,
										  const uint8_t ip1, const uint8_t ip2,
										  const uint8_t ip3, const uint8_t ip4 )
{
#if !defined(BIG_ENDIAN)
			*ip = ip1 + (ip2 << 8 ) + (ip3 << 16) + (ip4 << 24);
#else
			*ip =(ip1 << 24) + (ip2 << 16) + (ip3 << 8 ) + ip4;
#endif
}

/**
 * @return	1st byte of IPv4 address
 */
static __inline int libMU_IP_1(IPAddress_t ip)	{ return ( ip      & 0xFF); }
/**
 * @return	2nd byte of IPv4 address
 */
static __inline int libMU_IP_2(IPAddress_t ip)	{ return ((ip>>8)  & 0xFF); }
/**
 * @return	3rd byte of IPv4 address
 */
static __inline int libMU_IP_3(IPAddress_t ip)	{ return ((ip>>16) & 0xFF); }
/**
 * @return	4th byte of IPv4 address
 */
static __inline int libMU_IP_4(IPAddress_t ip)	{ return ((ip>>24) & 0xFF); }

/**
 * Set page for the local web server at port 80, that is used as a response when it receives
 * a GET command with the corresponding tag
 * @param tag		Page identifier name (e.g., "/index.html")
 * @param data		Web page data (if NULL then it erases the corresponding tag)
 * @param len		Web page data length (if 0 then it erases the corresponding tag)
 * @return			true if the page has been stored
 */
int libMU_Internet_Server_SetPage( char* tag, char* datos, int len );

/**
 * Set callback function for form parameter processing that is used as a response when it receives
 * a GET/POST command with the corresponding tag
 * @param tag		Command processing page identification name (e.g., "/cmd")
 * @param fun		Callback function to process form parameters
 * @return			true if the function has been stored
 * @see libMU_Internet_CommandProcessingFunction_t
 */
int libMU_Internet_Server_SetCommandProcessingInfo( char* tag, libMU_Internet_CommandProcessingFunction_t funcion );

/**
 * Get an integer parameter value from a form parameter list given
 * with the GET/POST method
 * @param cmd_params	GET/POST method parameter list
 * @param param_tag		Parameter to search (e.g. "x=")
 * @param result		Pointer to integer where the result will be stored if the parameter is found
 * @return				True if parameter found
 */
int libMU_Internet_GetFormIntegerValue( const char* cmd_params, const char* param_tag, int* value );

/**
 * Get a string parameter value from a form parameter list given
 * with the GET/POST method
 * @param cmd_params	GET/POST method parameter list
 * @param param_tag		Parameter to search (e.g. "x=")
 * @param result		Pointer to the character array where the result will be stored if the parameter is found
 * @param result_size	Maximum number of characters available at the given address pointer
 * @return				True if parameter found
 */
int libMU_Internet_GetFormStringValue( const char* cmd_params, const char* param_tag, char* value, int value_size );

/**
 * Obtain WiFi router IP address using telnet protocol
 * @return	WiFi Router IP address
 * @note 	This function has a very long timeout in the worst case (15s) 
 */
IPAddress_t	libMU_Internet_GetWiFiRouterIP( void );

/**
 * check if Ethernet link is OK
 * by reading register 1 of PHY interface
 * @return	true if link OK
 */
uint8_t	libMU_Ethernet_LinkOk(void);

#endif /*INTERNET_H_*/
/**
 * @}
 */
