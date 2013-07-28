/**
 * @addtogroup	libMU_Ethernet
 * Library for Ethernet access
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

#ifndef LIBMU_ETHERNET_H_
#define LIBMU_ETHERNET_H_

#include <stdint.h>
#include <lib/network_state.h>
#include <FreeRTOS.h>
#include <task.h>

/**
 * Constants
 */
/* Ethernet protocola management task parameters */
#define	LIBMU_ETHERNET_PRIORITY		(tskIDLE_PRIORITY + 2)
#define LIBMU_ETHERNET_STACK_SIZE	1024
#define LIBMU_ETHERNET_RATE_MS		50	/* Rate in ms */

/* Ethernet application task parameters */
#define	LIBMU_ETHERNET_APP_PRIORITY		(tskIDLE_PRIORITY)
#define LIBMU_ETHERNET_APP_STACK_SIZE	1024

/**
 * Ethernet frame management FreeRTOS task
 * @param pvParameters	Unused task parameters
 */
portTASK_FUNCTION( libMU_Ethernet_Task, pvParameters );

/**
 * check if Ethernet link is OK
 * by reading register 1 of PHY interface
 * @return	true if link OK
 */
uint8_t	libMU_Ethernet_LinkOk(void);

#endif/*LIBMU_ETHERNET_H_*/
/**
 * @}
 */
