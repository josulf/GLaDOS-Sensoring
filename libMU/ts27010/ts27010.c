/**
 * @addtogroup	libMU_TS27010
 * Librería para el protocol de multiplexacion 3GPP TS27.010 v10.0.0
 * crea puertos virtuales COM usando una linea serie
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2012/6/05
 * @copyright	BSDL
 ********************************************************************
 */

#include <stdint.h>
#include <string.h>
#include <libMU/serial_FreeRTOS.h>
#include <libMU/ts27010.h>
#include <libMU/random.h>
#include <libMU/leds.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <libMU/stdlib.h>
#include <libMU/serial/serial.port.h>

/**
 * Constants
 */
typedef enum _libMU_TS27010_Constants_t {
	LIBMU_TS27010_MAX_CHANNELS	= 64,		/* Maximum number of DLCI channels */
	LIBMU_TS27010_MAX_RETRIES	= 3,		/* How many times do we need to retry an AT command */
	LIBMU_TS27010_MAX_LINES		= 3,		/* Maximum number of response lines to process in search of an ok */
//	LIBMU_TS27010_CONNECT_RETRY	= 600000,	/* Timeout to try connecting to mux again (10 minutes) */
	LIBMU_TS27010_CONNECT_RETRY	= 5000,	/* Timeout to try connecting to mux again (10 minutes) */

	TS27010_BASIC_STARTSTOP		= 0xF9,		/* 9F in network order (LSB first) */
	TS27010_BASIC_MTU			= 128,		/* Size of the reception buffer */

	TS27010_ADVANCED_STARTSTOP	= 0x7E,
	TS27010_ADVANCED_ESCAPE		= 0x7D,		/* BE */
	TS27010_ADVANCED_XON		= 0x11,		/* or 0x91 */
	TS27010_ADVANCED_XOFF		= 0x13,		/* or 0x93 */
	TS27010_ADVANCED_XOO_MASK	= 0x7F,

	/* Various bit masks used throughout the different fields */
	TS27010_EA_MASK				= 0x01,		/* 1000 0000 EA: Extended Access bit mask */
	TS27010_CR_MASK				= 0x02,		/* 0100 0000 C/R: Command/Response bit mask */

	/* Multiplexer basic operation (P: refers to the P/F bit, poll/final */
	TS27010_CMD_PF_MASK			= 0x10,		/* 0000 1000 P/F bit mask */
	TS27010_CMD_SABM			= 0x3F,		/* 1111 P100 */
	TS27010_CMD_UA				= 0x63,		/* 1100 P110 */
	TS27010_CMD_DM_RESP			= 0x0F,		/* 1111 P000 */
	TS27010_CMD_DISC			= 0x53,		/* 1100 P010 */
	TS27010_CMD_UIH				= 0xEF,		/* 1111 P111 */
	TS27010_CMD_UI				= 0x03,		/* 1100 P000 */

	/* Multiplexer control channel commands (sent/received from DLCI 0) */
	TS27010_MUX_PN				= 0x81,		/* 1: Parameter Negotiation */
	TS27010_MUX_PSC				= 0x41,		/* 2: Power Saving Control */
	TS27010_MUX_CLD				= 0xC1,		/* 3: Multiplexed CLose Down */
	TS27010_MUX_TEST			= 0x21,		/* 4: Connection TEST */
	TS27010_MUX_FCON			= 0xA1,		/* 5: Flow Control ON */
	TS27010_MUX_FCOFF			= 0x61,		/* 6: Flow Control OFF */
	TS27010_MUX_MSC				= 0xE1,		/* 7: Modem Status Command (status) */
	TS27010_MUX_NSC				= 0x11,		/* 8: Non Supported Command response */
	TS27010_MUX_RPN				= 0x91,		/* 9: Remote Port Negotiation */
	TS27010_MUX_RLS				= 0x51,		/*10: Remote Line Status command */
	TS27010_MUX_SNC				= 0xD1,		/*11: Service Negotiation Command */

	TS27010_PARAM_TYPE			= 0,		/* Data frame type (0: UIH / 1: UI / 2: I) */
	TS27010_PARAM_CL			= 0,		/* Convergence layer ( Type Number - 1 ) */
	TS27010_PARAM_N1			= 98,		/* Size of the reception buffer (from ph8-p_atc_v2003.pdf page 84) */
	TS27010_PARAM_T1			= 10,		/* Acknowledge timeout in 10ms units (from 3GPP_27.007-v11.2.0.pdf page 23) */
	TS27010_PARAM_N2			= 3,		/* Maximum number of retransmissions (from 3GPP_27.007-v11.2.0.pdf page 23) */
	TS27010_PARAM_K				= 2,		/* Error recovery windows size (from 3GPP_27.007-v11.2.0.pdf page 23) */


} libMU_TS27010_Constants;


/**
 * Prototypes
 */
extern void DbgPrint( const char* msg);
void libMU_TS27010_RxHandlerBasic( void );
void libMU_TS27010_MuxManager( void* unused );
uint32_t libMU_TS27010_PrepareCMD( libMU_FIFO_t* queue, uint8_t dlci, uint8_t cmd );

/**
 * Connection status info array
 */
typedef struct _libMU_TS27010_DLCInfo_t {
	uint8_t			connected;	/* Data Link Channel connected signal */
	uint8_t			ui_enabled;	/* true if UI frames used, false if UIH frames used */
	uint16_t		mtu;		/* Maximum Transfer Unit (negotiated) */
	xQueueHandle	queue;		/* Reception queue */
} libMU_TS27010_DLCInfo_t;

libMU_TS27010_DLCInfo_t		libMU_TS27010_DLCInfo[LIBMU_TS27010_MAX_CHANNELS];
libMU_FIFO_t*				libMU_TS27010_ISRqueue = NULL;
volatile uint8_t			libMU_TS27010_UaReceived = 0;
volatile uint8_t			libMU_TS27010_DmReceived = 0;
extern volatile uint32_t	libMU_Serial_DataLost;
uint32_t					libMU_TS27010_MuxQueueSize;

/**
 * Reversed 8 bit CRC table for polynomial 0x07
 * @see		Page 51 of 3GPP TS 27.010 v10.0.0 (2011-03) Technical specification
 */
const uint8_t	libMU_TS27010_CRCtable[256] = { /*reversed, 8-bit, poly=0x07 */
	0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75,  0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
	0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69,  0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
	0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D,  0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
	0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51,  0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,

	0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05,  0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
	0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19,  0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
	0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D,  0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
	0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21,  0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,

	0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95,  0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
	0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89,  0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
	0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD,  0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
	0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1,  0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,

	0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5,  0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
	0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9,  0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
	0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD,  0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
	0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1,  0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF
};

/**
 * Initialize the TS 27.010 protocol variables/functions/task
 * @param	baudrate		Connection speed of the multiplexed serial communication in (bits/s)
 * @param	rxQueueSize		Size of the memory used for the global input buffer (allocated by FreeRTOS)
 * @return					true if initialization is correct
 * @note					The FreeRTOS scheduler has to be running for this function to work
 */
int		libMU_TS27010_Initialize( uint32_t baudrate, uint32_t rxQueueSize )
{
	libMU_Serial_Initialize( baudrate );
	if( !libMU_Serial_Initialized() ) return 0;
	if( xTaskCreate( libMU_TS27010_MuxManager, (signed char*)"TS27010",
					 configMINIMAL_STACK_SIZE*2, NULL, configMAX_PRIORITIES - 1, NULL ) != pdPASS ) {
		return 0;
	}
	libMU_TS27010_MuxQueueSize = rxQueueSize;
	libMU_Random_AddEntropy( baudrate );		/* For testing */
	libMU_Random_AddEntropy( rxQueueSize );
	libMU_Random_Initialize();
	return 1;
}

int	libMU_TS27010_SendATcmd( libMU_FIFO_t* queue, const char* at_cmd, const uint32_t len )
{
	uint8_t	retries, lines; char msg[80]; int time, timeout, num_read;
	for( retries = 0; retries < LIBMU_TS27010_MAX_RETRIES; retries++ ) {
		/* Send AT command */
		libMU_Serial_SendDataTS( queue, at_cmd, len );
		/*libMU_LED_Toggle( LED_2 ); */
		time = xTaskGetTickCount() + 5000 * portTICK_RATE_MS_INV;	/* Wait up to 5 secs for OK response */
		/* Wait response (3 lines max) */
		for( lines = 0; lines < LIBMU_TS27010_MAX_LINES*10; lines++ ) {
			timeout = ( time - xTaskGetTickCount() ) / portTICK_RATE_MS_INV;
			if( timeout < 0 ) break;
			num_read = libMU_Serial_GetUntilChar_FreeRTOS( msg, sizeof(msg), '\n', timeout );
			if( num_read == 0 ) continue;
			/*libMU_LED_Toggle( LED_3 ); */
			/*strupr( msg ); */
			if( strstr( msg, "OK\r\n" )!= NULL ) return 1;
			if( strstr( msg, "ERROR\r\n" )!= NULL ) return 0;
			if( strstr( msg, "+CME ERROR:" )!= NULL ) return 0;
		}
	}
	return 0;
}

/**
 * Data Link Channel 0: Multiplex connection management task
 */
void libMU_TS27010_MuxManager( void* unused )
{
	extern	xQueueHandle libMU_Serial_RxQueueFreeRTOS;
	/* From wm01_mux_drv_dev_guide_v017.pdf page 17 */
	const char* AT[] = {
			"AT\r\n",
			"AT\\Q3\r\n",
			"AT&S0\r\n",
			"AT+CMUX=?\r\n",
			"AT+CMUX=0\r\n"
	};
	static uint8_t	TxBuffer[256], TxISR_Buffer[100];
	enum _mux_t { MUX_CONNECTING, MUX_STARTED, MUX_STOPPED } muxState;
	uint8_t type, len, i, retries, frame[16], response;
	libMU_FIFO_t*	queue = libMU_FIFO_Initialize( TxBuffer, sizeof(TxBuffer), sizeof(char) );
	memset( libMU_TS27010_DLCInfo, 0, sizeof(libMU_TS27010_DLCInfo) );
	/*DbgPrint( "Starting TS27.010 MuxManager\r\n" ); */
	if( !libMU_Serial_SetupFreeRTOSHandler( libMU_TS27010_MuxQueueSize ) ) {
		/*DbgPrint( "Can not start FreeRTOS serial handler\r\n"); */
		for(;;) {}
	}
	muxState = MUX_CONNECTING;
	for(;;)
	{
		switch( muxState ) {
		case MUX_STOPPED:
			vTaskDelay( 500 * portTICK_RATE_MS_INV );
			libMU_LED_Off( LED_1 );
			break;
		case MUX_CONNECTING:
			/* Check if previous sends have ended */
			while( libMU_Serial_EndedSending() == 0 ) {
				libMU_Serial_StartSendingData();
				vTaskDelay( 10 * portTICK_RATE_MS_INV );
			}
			if( libMU_Serial_EndedSending() < 0 ) {
				/*DbgPrint( "Error in send queues\r\n" ); */
			}
			/* Test AT+CMUX support */
			for( i = 0; i < sizeof(AT)/sizeof(char*); i++ ) {
				response = libMU_TS27010_SendATcmd( queue, AT[i], strlen(AT[i]) );
				if( !response ) break;
			}
			if( !response ) {
				vTaskDelay( LIBMU_TS27010_CONNECT_RETRY * portTICK_RATE_MS_INV );	continue;
			}
			libMU_TS27010_DLCInfo[0].connected = 0;
			libMU_TS27010_DLCInfo[0].queue = libMU_Serial_RxQueueFreeRTOS;
			while( uxQueueMessagesWaiting( libMU_Serial_RxQueueFreeRTOS ) > 0 ) {
				xQueueReset( libMU_Serial_RxQueueFreeRTOS );
			}

			/* Set TS 27.010 handler */
			libMU_TS27010_ISRqueue = libMU_FIFO_Initialize( TxISR_Buffer, sizeof(TxISR_Buffer), sizeof(char) );
			libMU_Serial_SetRxHandlerFreeRTOS( libMU_TS27010_RxHandlerBasic );
			for( retries = 0; retries < LIBMU_TS27010_MAX_RETRIES; retries++ ) {
				if( libMU_TS27010_Connect( queue, 0, 256 ) ) {
					libMU_LED_On( LED_1 );
					muxState = MUX_STARTED;
					break;
				}
			}
			if( !libMU_TS27010_Connected( 0 ) ) {
				libMU_Serial_RestoreRxHandlerFreeRTOS();
				vTaskDelay( LIBMU_TS27010_CONNECT_RETRY * portTICK_RATE_MS_INV );
			}
			break;
		case MUX_STARTED:
			/* Wait for new mux cmd */
			while( xQueueReceive( libMU_TS27010_DLCInfo[0].queue, &type, portMAX_DELAY ) != pdTRUE ) {}
			while( xQueueReceive( libMU_TS27010_DLCInfo[0].queue, &len, portMAX_DELAY ) != pdTRUE ) {}
			len >>= 1;
			for( i = 0; i < len; i++ ) {
				while( xQueueReceive( libMU_TS27010_DLCInfo[0].queue, &frame[i], portMAX_DELAY ) != pdTRUE ) {}
			}
			switch( type & ~TS27010_CR_MASK ) {
			case TS27010_MUX_PN:
				break;
			}
			/* Mux commands not supported ??? */
			break;
		}

	}
}

/**
 * Prepare and send multiplexer PN command
 * @param queue		Queue to used for sending
 * @param dlci		Data Link Channel Identifier (Virtual COM port)
 */
void libMU_TS27010_MUX_SendPN( libMU_FIFO_t* queue, uint8_t dlci )
{
	uint8_t	frame[10];
	frame[0] = TS27010_MUX_PN | TS27010_CR_MASK;
	frame[1] = (8 << 1) | TS27010_EA_MASK;
	frame[2] = dlci;
	frame[3] = (TS27010_PARAM_CL<<4) | TS27010_PARAM_TYPE;		/* UIH frames and type 1 convergence layer */
	frame[4] = dlci;					/* Priority */
	frame[5] = TS27010_PARAM_T1;		/* T1 Ack timeout in 10ms units (default: 100ms = 10 x 10ms) */
	frame[6] = TS27010_PARAM_N1;		/* N1 frame size (default: 96) */
	frame[7] = TS27010_PARAM_N1>>8;		/* N1 frame size (high order 8 bits) */
	frame[8] = TS27010_PARAM_N2;		/* N2 max number of retries (default: 3) */
	frame[9] = TS27010_PARAM_K;			/* K  window size for recovery (default: 2) */
	libMU_TS27010_Send( queue, 0, frame, sizeof(frame) );
}

/**
 * Prepare and send multiplexer PN command
 * @param queue		Queue to used for sending
 * @param dlci		Data Link Channel Identifier (Virtual COM port)
 * @return			true if connection is ok
 */
int libMU_TS27010_MUX_ConnectionTest( libMU_FIFO_t* queue, uint32_t timeout )
{
	uint8_t	frame[10], frame_resp[10]; uint32_t i, len, len_resp;
	len = ( libMU_Random_GetValue() >> (32-3) ) + 3;
	frame[0] = TS27010_MUX_TEST | TS27010_CR_MASK;
	frame[1] = ((len - 2) << 1) | TS27010_EA_MASK;	/* Number of test bytes */
	for( i = 2; i < len; i++ ) {
		frame[i] = libMU_Random_GetValue() >> 24;
	}
	libMU_TS27010_Send( queue, 0, frame, len );
	len_resp = libMU_TS27010_GetData( 0, frame_resp, len, 1000 * portTICK_RATE_MS_INV );
	if( len > len_resp ) return 0;
	if( frame_resp[0] != (frame_resp[0] ^ TS27010_CR_MASK) ) return 0;
	for( i = 1; i < len; i++ ) {
		if( frame[i] != frame_resp[i] ) return 0;
	}
	return 1;
}

/**
 * Check if a Data Link Channel (Virtual COM port) is established with the other multiplexer device
 * @param dlci			Data Link Channel Identifier (Virtual COM port)
 * @return				true if channel is correctly established
 */
uint8_t		libMU_TS27010_Connected( uint8_t dlci )
{
	return libMU_TS27010_DLCInfo[ dlci ].connected;
}

/**
 * Establish a Data Link Channel (Virtual COM port) with the other multiplexer device
 * @param queue			Queue to use for thread safe buffering
 * @param dlci			Data Link Channel Identifier (Virtual COM port)
 * @param rxQueueSize	Size of the memory used for the channel input buffer (allocated by FreeRTOS)
 * @return				true if channel is correctly established
 */
int		libMU_TS27010_Connect( libMU_FIFO_t* queue, uint8_t dlci, int rxQueueSize )
{
	uint8_t	save_UaReceived = libMU_TS27010_UaReceived; uint32_t len;
	portTickType time = xTaskGetTickCount();
	len = libMU_TS27010_PrepareCMD( queue, dlci<<2 | TS27010_CR_MASK, TS27010_CMD_SABM );
	libMU_Serial_SendBuffer( queue, len );
	libMU_Serial_StartSendingData();
	while( ( xTaskGetTickCount() - time ) < 50 * portTICK_RATE_MS_INV ) {
		if( save_UaReceived != libMU_TS27010_UaReceived ) {
			libMU_TS27010_DLCInfo[ dlci ].connected = 1;
			if( libMU_TS27010_DLCInfo[ dlci ].queue == NULL ) {
				libMU_TS27010_DLCInfo[ dlci ].queue = xQueueCreate( rxQueueSize, sizeof(char) );
			}
			return 1;
		}
	}
	return 0;
}

/**
 * Send data to the specified data link channel
 * @param queue		Queue to use for thread safe buffering
 * @param dlci		Data Link Channel Identifier (Virtual COM port)
 * @param data		Pointer to data to send
 * @param len		Number of octets to send
 * @return			Number of octets sent
 * @note			Each thread should use a different queue otherwise it will not work.
 * 					This function will block if the queue is not big enough to hold all data or
 * 					the transmission buffer queue is full.
 */
int		libMU_TS27010_Send( libMU_FIFO_t* queue, int dlci, const uint8_t* data, uint32_t len )
{
	uint32_t i, block_len, frame_len, data_sent; uint8_t FCS, ui_enabled, c;
	/* Check if DLC is established */
	if( !libMU_TS27010_DLCInfo[ dlci ].connected ) return 0;
	if( dlci < 0 || dlci > 63 || libMU_TS27010_DLCInfo[ dlci ].queue == NULL ) return 0;
	/* Respect MTU for the given channel */
	block_len  = libMU_TS27010_DLCInfo[ dlci ].mtu;
	if( block_len > 0x7F )	frame_len = block_len + 7;
	else					frame_len = block_len + 6;
	if( libMU_FIFO_Size( queue ) < frame_len ) return 0;	/* queue is not big enough to hold one frame */
	ui_enabled = libMU_TS27010_DLCInfo[ dlci ].ui_enabled;
	data_sent = 0;
	/* Start sending */
	while( len > 0 ) {
		if( block_len > len ) block_len = len;
		len -= block_len;
		/* Wait until queue has enough space to hold the frame */
		while( libMU_FIFO_SpaceLeft( queue ) < frame_len ) { portYIELD(); }
		/* convert data into basic UIH packets */
		libMU_FIFO_PutChar( queue, TS27010_BASIC_STARTSTOP );
		libMU_FIFO_PutChar( queue, dlci );
		FCS = libMU_TS27010_CRCtable[ dlci ^ 0xFF ];
		if( ui_enabled ) {
			libMU_FIFO_PutChar( queue, TS27010_CMD_UIH );
			FCS = libMU_TS27010_CRCtable[ FCS ^ TS27010_CMD_UIH ];
		}else{
			libMU_FIFO_PutChar( queue, TS27010_CMD_UI );
			FCS = libMU_TS27010_CRCtable[ FCS ^ TS27010_CMD_UI ];
		}
		if( block_len > 0x7F ) {
			c = ((block_len & 0x7F) << 1);
			libMU_FIFO_PutChar( queue, c );
			FCS = libMU_TS27010_CRCtable[ FCS ^ c ];
			c = ( block_len >> 7 );
			libMU_FIFO_PutChar( queue, c );
			FCS = libMU_TS27010_CRCtable[ FCS ^ c ];
		}else{
			c = (( block_len & 0x7F ) << 1) | TS27010_EA_MASK;
			libMU_FIFO_PutChar( queue, c );
			FCS = libMU_TS27010_CRCtable[ FCS ^ c ];
		}
		for( i = 0; i < block_len; i++ ) {
			c = *data++;
			libMU_FIFO_PutChar( queue, c );
			if( ui_enabled ) FCS = libMU_TS27010_CRCtable[ FCS ^ c ];
		}
		libMU_FIFO_PutChar( queue, ~FCS );
		libMU_FIFO_PutChar( queue, TS27010_BASIC_STARTSTOP );
		/* Send the buffer */
		if( dlci == 0 ) {
			libMU_Serial_SendBuffer_HP( queue, frame_len );
		}else{
			libMU_Serial_SendBuffer( queue, frame_len );
		}
/*		data_sent += frame_len; */
		data_sent += block_len;
	}
	/* Start sending */
	libMU_Serial_StartSendingData();
	return data_sent;
}

/**
 * Function to read data from the channel until a newline character is received or
 * the data storage is full
 * @param	dlci	Data Link Channel Identifier (Virtual COM port)
 * @param	data	Pointer where the received data is stored
 * @param	size	Maximum size of the storage area
 * @return			number of characters read
 * @note			This function blocks the execution if there is not enough data
 */
int		libMU_TS27010_GetLine( int dlci, uint8_t* data, uint32_t size )
{
	uint32_t i; char c;
	if( dlci < 0 || dlci > 63 ) return 0;
	xQueueHandle queue = libMU_TS27010_DLCInfo[dlci].queue;
	if( queue == NULL ) return 0;
	size--;
	for( i = 0; i < size; ) {
		while( xQueueReceive( queue, &c, portMAX_DELAY ) != pdTRUE );
		data[i] = c; i++;
		if( c == '\n' ) break;
	}
	data[i] = 0;
	return i;
}

/**
 * Function to read exactly the given number of data from the channel
 * @param	dlci	Data Link Channel Identifier (Virtual COM port)
 * @param	data	Pointer where the received data is stored
 * @param	len		Number of characters to read
 * @param	timeout	Timeout to finish receiving data (in FreeRTOS units)
 * @return			number of characters read
 * @note			This function blocks the execution if there is not enough data
 */
int		libMU_TS27010_GetData( int dlci, uint8_t* data, uint32_t size, uint32_t timeout )
{
	uint32_t i; char c; uint32_t endtime; int timeleft;
	if( dlci < 0 || dlci > 63 ) return 0;
	xQueueHandle queue = libMU_TS27010_DLCInfo[dlci].queue;
	if( queue == NULL ) return 0;
	endtime = xTaskGetTickCount() + timeout;
	for( i = 0; i < size; ) {
		if( timeout != portMAX_DELAY ) {
			timeleft = endtime - xTaskGetTickCount();
			if( timeleft < 0 ) break;
		}else{
			timeleft = (int)portMAX_DELAY;
		}
		if( xQueueReceive( queue, &c, timeleft ) == pdTRUE ) { data[i] = c; i++; }
	}
	return i;
}

/**
 * Calculate Frame Check Sequence for the given message
 * @param	msg			Pointer to message
 * @param	len			Length of message
 * @return				FCS value
 */
uint8_t	libMU_TS27010_FCSCalculate( uint8_t* msg, uint32_t len )
{
	uint8_t	FCS=0xFF;	/* Init */
	while( len-- ) {
		FCS = libMU_TS27010_CRCtable[ FCS ^ msg[0] ]; msg++;
	}
	return (~FCS);		/* Ones complement */
}

/**
 * Check Frame Check Sequence for the given message
 * @param	msg			Pointer to message
 * @param	len			Length of message
 * @param	recv_FCS	Received FCS
 * @return				true if ok
 */
uint8_t	libMU_TS27010_FCSCheck( uint8_t* msg, uint32_t len, uint8_t recv_FCS )
{
	uint8_t	FCS=0xFF;		/* Init */
	while (len--) {
		FCS = libMU_TS27010_CRCtable[ FCS ^ msg[0] ]; msg++;
	}
	FCS = libMU_TS27010_CRCtable[ FCS ^ recv_FCS ];	/* Ones complement */
	return ( FCS == 0xCF );	/* 0xCF is the reversed order of 11110011 */
}

/**
 * Prepare and send multiplexer command/response
 * @param queue		Queue to use for thread safe buffering
 * @param dlci		Data Link Channel Identifier (Virtual COM port) & C/R bit & EA bit
 * @param cmd		Command to prepare
 * @return			Number of bytes in command/response
 */
uint32_t libMU_TS27010_PrepareCMD( libMU_FIFO_t* queue, uint8_t dlci, uint8_t cmd )
{
	uint8_t FCS;
	libMU_FIFO_PutChar( queue, TS27010_BASIC_STARTSTOP );
	dlci |= TS27010_EA_MASK;
	FCS = libMU_TS27010_CRCtable[ 0xFF^dlci ];	libMU_FIFO_PutChar( queue, dlci );
	FCS = libMU_TS27010_CRCtable[ FCS ^ cmd ];	libMU_FIFO_PutChar( queue, cmd );
	libMU_FIFO_PutChar( queue, ~FCS );
	libMU_FIFO_PutChar( queue, TS27010_BASIC_STARTSTOP );
	return 5;
}

void libMU_TS27010_ManageMuxLowLevel( uint8_t dlci, uint8_t cr )
{
	uint32_t len;
	switch( cr ){
	case TS27010_CMD_DISC:	/* TS27010_CMD_PF_MASK */
		if( libMU_TS27010_DLCInfo[ dlci >> 2 ].connected ) {
			libMU_TS27010_DLCInfo[ dlci >> 2 ].connected = 0;
			len = libMU_TS27010_PrepareCMD( libMU_TS27010_ISRqueue, dlci ^ TS27010_CR_MASK,
											TS27010_CMD_UA | TS27010_CMD_PF_MASK );
		}else{
			len = libMU_TS27010_PrepareCMD( libMU_TS27010_ISRqueue, dlci ^ TS27010_CR_MASK,
											TS27010_CMD_DM_RESP | TS27010_CMD_PF_MASK );
		}
		if( (dlci >> 2) == 0 ) {
			libMU_Serial_SendBuffer_HP_FromISR( libMU_TS27010_ISRqueue, len );
			/* Disconnect the TS27.010 handler and connect the FreeRTOS one */
			libMU_Serial_RestoreRxHandlerFreeRTOS();
			/*libMU_BufferQueue_Reset( libMU_Serial_TxQueue_LowPri ); */
		}
		break;
	case TS27010_CMD_SABM:	/* TS27010_CMD_PF_MASK */
		libMU_TS27010_DLCInfo[ dlci >> 2 ].connected = 1;
		len = libMU_TS27010_PrepareCMD( libMU_TS27010_ISRqueue, dlci ^ TS27010_CR_MASK,
										TS27010_CMD_UA | TS27010_CMD_PF_MASK );
		libMU_Serial_SendBuffer_HP_FromISR( libMU_TS27010_ISRqueue, len );
		break;
	case TS27010_CMD_UA | TS27010_CMD_PF_MASK:
	case TS27010_CMD_UA:
		libMU_TS27010_UaReceived++;
		break;
	case TS27010_CMD_DM_RESP:
	case TS27010_CMD_DM_RESP | TS27010_CMD_PF_MASK:
		libMU_TS27010_DmReceived++;
		break;
	default:
		/* Incorrect command or empty UI/UIH frames, discard */
		break;
	}
}

typedef enum _TS27010_BasicState_t {
	TS27010_BASIC_STATE_FLAG_EXPECTED,
	TS27010_BASIC_STATE_WAITING_ADDRESS,
	TS27010_BASIC_STATE_EXTADDRESS,
	TS27010_BASIC_STATE_CMD,
	TS27010_BASIC_STATE_LEN,
	TS27010_BASIC_STATE_LEN_EXT,
	TS27010_BASIC_STATE_DATA,
	TS27010_BASIC_STATE_FCS
} TS27010_BasicState_t;

typedef enum _TS27010_AdvancedState_t {
	TS27010_ADVANCED_STATE_WAITING_ADDRESS,
	TS27010_ADVANCED_STATE_EXTADDRESS,
	TS27010_ADVANCED_STATE_CMD,
	TS27010_ADVANCED_STATE_DATA		/* FCS included here */
} TS27010_AdvancedState_t;

static uint16_t				packet_size, number_received;
static uint8_t				fsm_dlci, fsm_cr;
static TS27010_BasicState_t	fsm_state = TS27010_BASIC_STATE_WAITING_ADDRESS;
static xQueueHandle			fsm_dlci_queue;
static uint8_t				fsm_data[TS27010_BASIC_MTU];
static uint8_t				fsm_FCS, fsm_ui_enabled;

void	libMU_TS27010_BasicRxStateMachine( uint8_t c )
{
	switch( fsm_state ) {
	case TS27010_BASIC_STATE_WAITING_ADDRESS:
		if( c != TS27010_BASIC_STARTSTOP ) {
			fsm_FCS = libMU_TS27010_CRCtable[ 0xFF ^ c ];
			if( c & TS27010_EA_MASK ) {
				fsm_dlci = c; c >>= 2;
				fsm_dlci_queue = libMU_TS27010_DLCInfo[ c ].queue;
				fsm_ui_enabled = libMU_TS27010_DLCInfo[ c ].ui_enabled;
				fsm_state = TS27010_BASIC_STATE_CMD;
			}else{
				fsm_state = TS27010_BASIC_STATE_EXTADDRESS;
			}
		}
		break;
	case TS27010_BASIC_STATE_EXTADDRESS:
		fsm_dlci = (uint8_t)(-1);
		fsm_dlci_queue = NULL;
		fsm_ui_enabled = 0;
		fsm_state = TS27010_BASIC_STATE_CMD;
		break;
	case TS27010_BASIC_STATE_CMD:
		fsm_FCS = libMU_TS27010_CRCtable[ fsm_FCS ^ c ];
		fsm_cr = c;
		c &=~TS27010_CMD_PF_MASK;
		if( c == TS27010_CMD_UI || c == TS27010_CMD_UIH )	fsm_state = TS27010_BASIC_STATE_LEN;
		else												fsm_state = TS27010_BASIC_STATE_FCS;
		break;
	case TS27010_BASIC_STATE_LEN:
		fsm_FCS = libMU_TS27010_CRCtable[ fsm_FCS ^ c ];
		packet_size = (c >> 1); number_received = 0;
		if( TS27010_BASIC_MTU < packet_size ) fsm_dlci_queue = NULL; /* Discard: MTU not respected */
		if( c & TS27010_EA_MASK ) {
			if( packet_size != 0)	fsm_state = TS27010_BASIC_STATE_DATA;
			else					fsm_state = TS27010_BASIC_STATE_FCS;
		}else{
			fsm_state = TS27010_BASIC_STATE_LEN_EXT;
		}
		break;
	case TS27010_BASIC_STATE_LEN_EXT:
		fsm_FCS = libMU_TS27010_CRCtable[ fsm_FCS ^ c ];
		packet_size |= (((uint16_t)c)<<7);
		if( TS27010_BASIC_MTU < packet_size ) fsm_dlci_queue = NULL; /* Discard: MTU not respected */
		if( packet_size != 0)	fsm_state = TS27010_BASIC_STATE_DATA;
		else					fsm_state = TS27010_BASIC_STATE_FCS;
		break;
	case TS27010_BASIC_STATE_DATA:
		if( fsm_dlci_queue != NULL ) {
			fsm_data [ number_received ] = c;
			if( fsm_ui_enabled ) fsm_FCS = libMU_TS27010_CRCtable[ fsm_FCS ^ c ];
		}
		number_received++;
		if( number_received == packet_size ) {
			fsm_state = TS27010_BASIC_STATE_FCS;
		}
		break;
	case TS27010_BASIC_STATE_FCS:
		if( (fsm_FCS ^ c) != 0xFF ) fsm_dlci_queue = NULL;	/* Discard */
		fsm_state = TS27010_BASIC_STATE_FLAG_EXPECTED;
		break;
	case TS27010_BASIC_STATE_FLAG_EXPECTED:
		if( c == TS27010_BASIC_STARTSTOP ) {
			if( packet_size > 0 && libMU_TS27010_DLCInfo[ fsm_dlci>>2 ].connected  ) {	/* TS27010_CMD_UI || TS27010_CMD_UIH */
				if( fsm_dlci_queue != NULL ) {
					int i; portBASE_TYPE woken;
					for( i = 0; i < number_received; i++ ) {
						if( xQueueSendFromISR( fsm_dlci_queue, &fsm_data[i], &woken ) != pdTRUE )
							libMU_Serial_DataLost++;
					}
				}
			}else{
				/* Manage basic command responses */
				libMU_TS27010_ManageMuxLowLevel( fsm_dlci, fsm_cr );
			}
			fsm_state = TS27010_BASIC_STATE_WAITING_ADDRESS;
		}
		fsm_dlci_queue = NULL;
		break;
	}
}

/**
 * Interrupt service routine handler for basic TS 27.010 protocol communications
 */
void	libMU_TS27010_RxHandlerBasic( void )
{
	uint8_t c;
	do{
		c = libMU_Serial_GetCharHW();
		libMU_TS27010_BasicRxStateMachine( c );
	}while( !libMU_Serial_IsRxFifoEmpty() );
}

/**
 * Obtain the reception queue for the given Data Link Channel (Virtual COM port)
 * @param dlci			Data Link Channel Identifier (Virtual COM port)
 * @return				Reception queue handle for the virtual port
 */
xQueueHandle	libMU_TS27010_GetQueue( uint8_t dlci )
{
	if( dlci > 63 ) return NULL;
	return libMU_TS27010_DLCInfo[ dlci ].queue;
}

/**
 * @}
 */
