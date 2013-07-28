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
#include "ethernet.h"
#include <semphr.h>
#include <stdio.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/hw_ethernet.h>
#include <driverlib/ethernet.h>
#include <driverlib/gpio.h>
#include <driverlib/flash.h>
#include <driverlib/interrupt.h>
#include <uip/uip.h>
#include <uip/uip_arp.h>
#include "uip_app/protocol_switcher_app.h"

/************************************************/
/* Constants									*/
/************************************************/
#define	ETH_PACKET_RX_BIT	0
#define	ETH_TIMEOUT_BIT		1
#define	UIP_BUF_ETH_ADR		((struct uip_eth_hdr *)&uip_buf[0])

/* UIP Timers (in MS) */
#define UIP_PERIODIC_TIMER_MS	500
#define UIP_ARP_TIMER_MS		10000

/************************************************/
/* Global variables								*/
/************************************************/
xSemaphoreHandle	libMU_Ethernet_PacketReceived = NULL;
uint8_t				libMU_Ethernet_InitOk = false;
static uint32_t		lPeriodicTimer;
static uint32_t		lARPTimer;

/************************************************/
/* Prototypes									*/
/************************************************/
static uint8_t	libMU_Ethernet_CheckConnection(void);
static void		libMU_Ethernet_InitializeHW_and_UIP(void);
static void		libMU_Ethernet_ExecuteUIPLoop(void);
static void		libMU_Ethernet_InterruptHandler(void);

/**
 * check if ethernet link is OK
 * by reading register 1 of PHY interface
 * @return	true if link OK
 */
uint8_t	libMU_Ethernet_LinkOk(void)
{
    uint32_t ulTemp = EthernetPHYRead(ETH_BASE, PHY_MR1);
    return (ulTemp & PHY_MR1_LINK);
}

/**
 * Ethernet frame management FreeRTOS task
 * @param pvParameters	Unused task parameters
 */
portTASK_FUNCTION( libMU_Ethernet_Task, pvParameters )
{
	portTickType execTimePrv, execTime, elapsedTime, connCheckTime = 0;

	/* The parameters are not used */
	( void ) pvParameters;
	
    /* We are using the semaphore for synchronisation so we create a binary
    semaphore rather than a mutex.  We must make sure that the interrupt
    does not attempt to use the semaphore before it is created! */
    vSemaphoreCreateBinary( libMU_Ethernet_PacketReceived );

	/* Initialize Ethernet HW */
	libMU_Ethernet_InitializeHW_and_UIP();

	execTimePrv = xTaskGetTickCount();

	for(;;) {	/* Forever loop */
		/* Check if we have received a packet */
#if portTICK_RATE_MS > 0
		xSemaphoreTake( libMU_Ethernet_PacketReceived, LIBMU_ETHERNET_RATE_MS / portTICK_RATE_MS );
#else
		xSemaphoreTake( libMU_Ethernet_PacketReceived, LIBMU_ETHERNET_RATE_MS * portTICK_RATE_MS_INV );
#endif
		/* Get time since previous execution  */
		execTime = xTaskGetTickCount();
#if portTICK_RATE_MS > 0
		elapsedTime += (execTime - execTimePrv) * portTICK_RATE_MS;	/* ticks -> ms */
#else
		elapsedTime += (execTime - execTimePrv) / portTICK_RATE_MS_INV;	/* ticks -> ms */
#endif
		execTimePrv = execTime;

		/* Update UIP timers and UIP Main loop */
		lPeriodicTimer += elapsedTime;
		lARPTimer += elapsedTime;
		libMU_Ethernet_ExecuteUIPLoop();

		/* Connection check loop (every 1s) */
		connCheckTime += elapsedTime;
		if( connCheckTime > 1000 || network_state == NETWORK_NO_CONNECTION ) {
			connCheckTime = 0;
			libMU_Ethernet_CheckConnection();
		}
	}
}

static void libMU_Ethernet_InitializeHW_and_UIP(void)
{
    /* Enable and Reset the Ethernet Controller. */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH);
    SysCtlPeripheralReset(SYSCTL_PERIPH_ETH);

    /* Enable Port F for Ethernet LEDs
     * LED0 - bit 3   Output
     * LED1 - bit 2   Output */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);

#if 0
    /* Configure SysTick for a periodic interrupt. */
    SysTickPeriodSet(SysCtlClockGet() / SYSTICKHZ);
    SysTickEnable();
    SysTickIntEnable();	/* Interrupt every second and timer */
#endif
    
    /* Initialize the Ethernet Controller and disable all Ethernet Controller
       interrupt sources. */
    EthernetIntDisable(ETH_BASE, (ETH_INT_PHY | ETH_INT_MDIO | ETH_INT_RXER |
                       ETH_INT_RXOF | ETH_INT_TX | ETH_INT_TXER | ETH_INT_RX));
    unsigned long ulTemp = EthernetIntStatus(ETH_BASE, false);
    EthernetIntClear(ETH_BASE, ulTemp);
	IntRegister( INT_ETH, libMU_Ethernet_InterruptHandler );

    /* Initialize the Ethernet Controller for operation. */
    EthernetInitExpClk(ETH_BASE, SysCtlClockGet());
    
    /* Configure the Ethernet Controller for normal operation
     * - Full Duplex
     * - TX CRC Auto Generation
     * - TX Padding Enabled */
    EthernetConfigSet(ETH_BASE, (ETH_CFG_TX_DPLXEN | ETH_CFG_TX_CRCEN |
                                 ETH_CFG_TX_PADEN));	

	/* Get MAC address from USER0 & USER1 Flash memory */
    union {
    	uint32_t	user[2];
    	uint8_t		data[8];
    } var;
    struct uip_eth_addr macAddr;
    FlashUserGet( (unsigned long*)&var.user[0], (unsigned long*)&var.user[1] );
    if((var.user[0] == 0xffffffffu) || (var.user[1] == 0xffffffffu))
    {
        for(;;) {}	/* Incorrect MAC Address in Flash Memory */
    }
    /* Convert the 24/24 split MAC address from NV ram into a 32/16 split MAC
     * address needed to program the hardware registers, then program the MAC
     * address into the Ethernet Controller registers. */
    macAddr.addr[0] = var.data[0]; macAddr.addr[1] = var.data[1]; macAddr.addr[2] = var.data[2];
    macAddr.addr[3] = var.data[4]; macAddr.addr[4] = var.data[5]; macAddr.addr[5] = var.data[6];
#if 0
	macAddr.addr[0] = ((var.user[0] >>  0) & 0xff);
	macAddr.addr[1] = ((var.user[0] >>  8) & 0xff);
	macAddr.addr[2] = ((var.user[0] >> 16) & 0xff);
	macAddr.addr[3] = ((var.user[1] >>  0) & 0xff);
	macAddr.addr[4] = ((var.user[1] >>  8) & 0xff);
	macAddr.addr[5] = ((var.user[1] >> 16) & 0xff);
#endif

    /* Configure the hardware MAC address for Ethernet Controller filtering of
     * incoming packets. */
    EthernetMACAddrSet(ETH_BASE, (unsigned char *)macAddr.addr);

    /* Initialize the uIP TCP/IP stack. */
	uip_ipaddr_t ipaddr;
    uip_setethaddr(macAddr);
#ifdef USE_STATIC_IP
    uip_ipaddr(&ipaddr, DEFAULT_IPADDR0, DEFAULT_IPADDR1, DEFAULT_IPADDR2, DEFAULT_IPADDR3);
    uip_sethostaddr(&ipaddr);
    uip_ipaddr(&ipaddr, DEFAULT_NETMASK0, DEFAULT_NETMASK1, DEFAULT_NETMASK2, DEFAULT_NETMASK3);
    uip_setnetmask(&ipaddr);

    uip_ipaddr(&ipaddr, DEFAULT_IPADDR0, DEFAULT_IPADDR1, DEFAULT_IPADDR2, 1);
    uip_setdraddr(&ipaddr);

    network_state = NETWORK_IP_OK;
#else
    uip_ipaddr(&ipaddr, 0, 0, 0, 0);
    uip_sethostaddr(&ipaddr);
    uip_setnetmask(&ipaddr);
    uip_setdraddr(&ipaddr);
    network_state = NETWORK_IP_MISSING;
#endif

    uip_init();
	lPeriodicTimer = 0;
	lARPTimer = 0;
    /* Initialize all protocols */
    protocol_switcher_app_init();

    /* Wait for cable connection */
    while( !libMU_Ethernet_CheckConnection() ) {}

    /* Enable the Ethernet Controller, the interrupt controller and the RX packet interrupt source. */
    EthernetEnable(ETH_BASE);
    IntEnable(INT_ETH);
    EthernetIntEnable(ETH_BASE, ETH_INT_RX);
}

static uint8_t	libMU_Ethernet_CheckConnection(void)
{
    /* Return if the link is not active. */
    if( !libMU_Ethernet_LinkOk() )
    {
    	/* Disable the Ethernet Controller. */
        network_state = NETWORK_NO_CONNECTION;
    	libMU_Ethernet_InitOk = false;
    	return 0;
    }
    if(libMU_Ethernet_InitOk) return 1;
	libMU_Ethernet_InitOk = true;
	
#ifndef USE_STATIC_IP
    /* Do a new DCHP client request */
    dhcpc_request();
//    network_state = NETWORK_IP_MISSING;
#endif
    return 1;
}

static void libMU_Ethernet_ExecuteUIPLoop()
{
	long lPacketLength = 0;

    /* Check for an RX Packet and read it */
    lPacketLength = EthernetPacketGetNonBlocking(ETH_BASE, uip_buf,
                                                 sizeof(uip_buf));
    if(lPacketLength > 0)
    {
        /* Set uip_len for uIP stack usage */
        uip_len = (unsigned short)lPacketLength;

        /* Clear the RX Packet event and re-enable RX Packet interrupts */
        EthernetIntEnable(ETH_BASE, ETH_INT_RX);

        /* Process incoming IP packets here */
        if(UIP_BUF_ETH_ADR->type == HTONS(UIP_ETHTYPE_IP))
        {
            uip_arp_ipin();
            uip_input();

            /* If the above function invocation resulted in data that
             * should be sent out on the network, the global variable
             * uip_len is set to a value > 0 */
            if(uip_len > 0)
            {
                uip_arp_out();
                EthernetPacketPut(ETH_BASE, uip_buf, uip_len);
                uip_len = 0;
            }
        }
        /* Process incoming ARP packets here */
        else if(UIP_BUF_ETH_ADR->type == HTONS(UIP_ETHTYPE_ARP))
        {
            uip_arp_arpin();

            /* If the above function invocation resulted in data that
             * should be sent out on the network, the global variable
             * uip_len is set to a value > 0 */
            if(uip_len > 0)
            {
                EthernetPacketPut(ETH_BASE, uip_buf, uip_len);
                uip_len = 0;
            }
        }
    }		
    /* Process TCP/IP Periodic Timer here */
    unsigned long ulTemp;
    if(lPeriodicTimer > UIP_PERIODIC_TIMER_MS)
    {
        lPeriodicTimer = 0;
        for(ulTemp = 0; ulTemp < UIP_CONNS; ulTemp++)
        {
            uip_periodic(ulTemp);

            /* If the above function invocation resulted in data that
             * should be sent out on the network, the global variable
             * uip_len is set to a value > 0 */
            if(uip_len > 0)
            {
                uip_arp_out();
                EthernetPacketPut(ETH_BASE, uip_buf, uip_len);
                uip_len = 0;
            }
        }

#if UIP_UDP
        for(ulTemp = 0; ulTemp < UIP_UDP_CONNS; ulTemp++)
        {
            uip_udp_periodic(ulTemp);

            /* If the above function invocation resulted in data that
             * should be sent out on the network, the global variable
             * uip_len is set to a value > 0 */
            if(uip_len > 0)
            {
                uip_arp_out();
                EthernetPacketPut(ETH_BASE, uip_buf, uip_len);
                uip_len = 0;
            }
        }
#endif /* UIP_UDP */
    }

    /* Process ARP Timer here */
    if(lARPTimer > UIP_ARP_TIMER_MS)
    {
        lARPTimer = 0;
        uip_arp_timer();
    }
}

/**
 * The interrupt handler for the Ethernet interrupt
 */
/* interrupt */
static void libMU_Ethernet_InterruptHandler(void)
{
    unsigned long ulTemp;

    /* Read and Clear the interrupt */
    ulTemp = EthernetIntStatus(ETH_BASE, false);
    EthernetIntClear(ETH_BASE, ulTemp);

    /* Check to see if an RX Interrupt has happened */
    if(ulTemp & ETH_INT_RX)
    {
        /* Disable Ethernet RX Interrupt */
        EthernetIntDisable(ETH_BASE, ETH_INT_RX);
        
        /* Do processing in the Loop_UIP() */
        portBASE_TYPE xHigherPriorityTaskWoken;
        xSemaphoreGiveFromISR( libMU_Ethernet_PacketReceived, &xHigherPriorityTaskWoken );
    }
}

/**
 * When using the timer module in UIP, this function is required to return
 * the number of ticks.  Note that the file "clock-arch.h" must be provided
 * by the application, and define CLOCK_CONF_SECONDS as the number of ticks
 * per second, and must also define the typedef "clock_time_t"
 */
#include <uip/clock.h>
clock_time_t clock_time(void)
{
    return((clock_time_t)xTaskGetTickCount());
}

/**
 * @}
 */
